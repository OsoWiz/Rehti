#include "PipelineManager.hpp"
#include "Logger.hpp"
#include "GraphicsUtils.hpp"

std::vector<VkVertexInputAttributeDescription> PipelineShaderData::getVertexAttributes(uint32_t binding) const
{
	if (!vertexShaderData.has_value())
	{
		Logger::warning("No data in vertex shader");
		return {};
	}

	std::vector<VkVertexInputAttributeDescription> result;
	uint32_t offset = 0;
	uint32_t location = 0;
	for (const auto& [attribute, format] : vertexShaderData.value().inputAttributes)
	{
		VkVertexInputAttributeDescription desc{};
		desc.binding = binding;
		desc.location = location;
		desc.format = format;
		desc.offset = offset;
		VertexAttributeInfo info = getAttributeInfo(attribute);
		offset += info.size;
		result.push_back(desc);
		location++;
	}
	return result;
}

std::vector<VkDescriptorSetLayout> PipelineShaderData::getDescriptorSetLayouts() const 
{
	std::array<VkDescriptorSetLayout, MAX_DESCRIPTOR_SETS> merged{};
	merged.fill(VK_NULL_HANDLE);

	auto mergeStageLayouts = [&merged](const std::optional<CompiledShaderData>& stage)
		{
			if (!stage.has_value())
				return;

			for (size_t set = 0; set < stage->descriptorSetLayouts.size(); ++set)
			{
				const VkDescriptorSetLayout layout = stage->descriptorSetLayouts[set];
				if (layout == VK_NULL_HANDLE)
					continue;

				if (merged[set] == VK_NULL_HANDLE)
				{
					merged[set] = layout;
				}
				else if (merged[set] != layout)
				{
					Logger::warning("Descriptor set layout mismatch across shader stages at set " + std::to_string(set));
				}
			}
		};

	mergeStageLayouts(vertexShaderData);
	mergeStageLayouts(tessControlShaderData);
	mergeStageLayouts(tessEvalShaderData);
	mergeStageLayouts(geometryShaderData);
	mergeStageLayouts(fragmentShaderData);

	size_t highestUsedSet = 0;
	bool hasAny = false;
	for (size_t i = 0; i < merged.size(); ++i)
	{
		if (merged[i] != VK_NULL_HANDLE)
		{
			hasAny = true;
			highestUsedSet = i;
		}
	}

	if (!hasAny)
		return {};

	for (size_t i = 0; i <= highestUsedSet; ++i)
	{
		if (merged[i] == VK_NULL_HANDLE)
		{
			Logger::warning("Descriptor set layout gap detected before highest used set");
			return {};
		}
	}

	return std::vector<VkDescriptorSetLayout>(merged.begin(), merged.begin() + highestUsedSet + 1);
}

std::vector<VkPushConstantRange> PipelineShaderData::getPushConstantRanges() const
{
	std::vector<VkPushConstantRange> ranges;

	auto addPushConstantRanges = [&ranges](const std::vector<VkPushConstantRange>& newRanges)
		{
			for (const auto& newRange : newRanges)
			{
				bool merged = false;
				for (auto& existingRange : ranges)
				{
					// if they are the same range, merge them
					if (existingRange.offset == newRange.offset && existingRange.size == newRange.size)
					{
						existingRange.stageFlags |= newRange.stageFlags;
						merged = true;
						break;
					}
				}
				if (!merged)
				{
					ranges.push_back(newRange);
				}
			}
		};

	if (vertexShaderData.has_value())
	{
		addPushConstantRanges(vertexShaderData->pushConstantRanges);
	}
	if (tessControlShaderData.has_value())
	{
		addPushConstantRanges(tessControlShaderData->pushConstantRanges);
	}
	if (tessEvalShaderData.has_value())
	{
		addPushConstantRanges(tessEvalShaderData->pushConstantRanges);
	}
	if (geometryShaderData.has_value())
	{
		addPushConstantRanges(geometryShaderData->pushConstantRanges);
	}
	if (fragmentShaderData.has_value())
	{
		addPushConstantRanges(fragmentShaderData->pushConstantRanges);
	}

	return ranges;
}

std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderData::getShaderStageInfos() const
{
	std::vector<VkPipelineShaderStageCreateInfo> stages;
	if (vertexShaderData.has_value())
	{
		stages.push_back(vertexShaderData->getShaderStageInfo());
	}
	if (tessControlShaderData.has_value())
	{
		stages.push_back(tessControlShaderData->getShaderStageInfo());
	}
	if (tessEvalShaderData.has_value())
	{
		stages.push_back(tessEvalShaderData->getShaderStageInfo());
	}
	if (geometryShaderData.has_value())
	{
		stages.push_back(geometryShaderData->getShaderStageInfo());
	}
	if (fragmentShaderData.has_value())
	{
		stages.push_back(fragmentShaderData->getShaderStageInfo());
	}

	return stages;
}

uint32_t PipelineShaderData::getStride() const
{
	if (!vertexShaderData.has_value())
	{
		std::cerr << "Error: No vertex shader currently set!" << std::endl;
		return {};
	}
	uint32_t stride = 0;
	for (const auto& [attribute, format] : vertexShaderData.value().inputAttributes)
	{
		VertexAttributeInfo info = getAttributeInfo(attribute);
		stride += info.size;
	}
	return stride;
}

VertexAttributeFlags PipelineShaderData::getAttributes() const
{
	if (!vertexShaderData.has_value())
	{
		std::cerr << "Error: No vertex shader currently set!" << std::endl;
		return {};
	}
	VertexAttributeFlags flags = VertexAttributeFlags::NONE;
	for (const auto& [attribute, format] : vertexShaderData.value().inputAttributes)
	{
		flags |= attribute;
	}
	return flags;
}

PipelineManager::PipelineManager(VkDevice& logDevice)
	: logDevice(logDevice)
{
}

PipelineManager::~PipelineManager()
{
	for (const auto& pipelinedata : pipelines)
	{
		vkDestroyPipeline(logDevice, pipelinedata.pipeline, nullptr);
	}
}

CompiledPipelineData PipelineManager::createPipeline(const PipelineShaderData& pipelineShaders, const GraphicsPipelineConfig& config, const PipelineCreationDetails& details)
{
	CompiledPipelineData compiledPipeline{};

	std::vector<VkVertexInputAttributeDescription> attributeDescs = Mapping::getVertexAttributeDescriptions(config.vertexShader, 0);
	std::vector<VkVertexInputBindingDescription> bindingDescs{};
	if (details.interleavedVertexData)
	{
		VkVertexInputBindingDescription bindingDesc{};
		bindingDesc.binding = 0;
		bindingDesc.stride = pipelineShaders.getStride();
		bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		bindingDescs.push_back(bindingDesc);
	}
	else
	{
		uint32_t binding = 0;
		for (const auto& attribute : attributeDescs)
		{
			VkVertexInputBindingDescription bindingDesc{};
			bindingDesc.binding = binding;
			VertexAttributeInfo info = getAttributeInfo(attribute.format);
			bindingDesc.stride = info.size;
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescs.push_back(bindingDesc);
			binding++;
		}
	}

	VkPipelineVertexInputStateCreateInfo vertInputInfo{};
	vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInputInfo.vertexBindingDescriptionCount = bindingDescs.size();
	vertInputInfo.pVertexBindingDescriptions = bindingDescs.data();
	vertInputInfo.vertexAttributeDescriptionCount = attributeDescs.size();
	vertInputInfo.pVertexAttributeDescriptions = attributeDescs.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	std::vector<VkDynamicState> dynamicStates = { // always include at least viewport and scissor as dynamic states
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineViewportStateCreateInfo viewportInfo{};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = nullptr;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = nullptr;

	VkPipelineRasterizationStateCreateInfo rasterInfo{};
	rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterInfo.depthClampEnable = VK_FALSE;
	rasterInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterInfo.polygonMode = Mapping::toVkType(config.rasterizationConfig.polygonFillMode);
	rasterInfo.lineWidth = 1.0f;
	rasterInfo.cullMode = Mapping::toVkType(config.rasterizationConfig.cullMode);
	rasterInfo.frontFace = Mapping::toVkType(config.rasterizationConfig.frontFace); // This is because y is flipped in perspective matrix
	rasterInfo.depthBiasEnable = VK_FALSE;
	rasterInfo.depthBiasConstantFactor = 0.f;
	rasterInfo.depthBiasClamp = 0.f;
	rasterInfo.depthBiasSlopeFactor = 0.f;

	VkPipelineMultisampleStateCreateInfo multInfo{};
	multInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multInfo.sampleShadingEnable = VK_FALSE;
	multInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multInfo.minSampleShading = 1.f;
	multInfo.pSampleMask = nullptr;
	multInfo.alphaToCoverageEnable = VK_FALSE;
	multInfo.alphaToOneEnable = VK_FALSE;

	// Vk depth and stencil testing info here

	// Colorblending
	VkPipelineColorBlendAttachmentState colorBlendState{};
	colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendState.blendEnable = VK_FALSE;
	// TODO configure alpha blending to this struct later

	VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.logicOpEnable = VK_FALSE;
	colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // optional
	colorBlendInfo.attachmentCount = 1;
	colorBlendInfo.pAttachments = &colorBlendState;
	// Constants optional

	// Layout info for literally only push constants and descriptor sets
	VkPipelineLayoutCreateInfo pipelinelayoutInfo{};
	pipelinelayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	std::vector<VkDescriptorSetLayout> descLayouts = pipelineShaders.getDescriptorSetLayouts();
	pipelinelayoutInfo.setLayoutCount = static_cast<uint32_t>(descLayouts.size());
	pipelinelayoutInfo.pSetLayouts = descLayouts.data();
	std::vector<VkPushConstantRange> pushConstants = pipelineShaders.getPushConstantRanges();
	pipelinelayoutInfo.pushConstantRangeCount = pushConstants.size();
	pipelinelayoutInfo.pPushConstantRanges = pushConstants.data();

	// Create pipelinelayout for the given object
	VkPipelineLayout newLayout;
	if (vkCreatePipelineLayout(logDevice, &pipelinelayoutInfo, nullptr, &newLayout))
		throw std::runtime_error("Pipeline layout creation failed");

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = static_cast<VkBool32>(config.depthStencilConfig.depthTestEnable);
	depthStencilInfo.depthWriteEnable = VK_TRUE;
	depthStencilInfo.depthCompareOp = static_cast<VkCompareOp>(config.depthStencilConfig.depthCompareOp); // this is sus but should work.
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.minDepthBounds = config.depthStencilConfig.depthBounds.first;
	depthStencilInfo.maxDepthBounds = config.depthStencilConfig.depthBounds.second;
	depthStencilInfo.stencilTestEnable = static_cast<VkBool32>(config.depthStencilConfig.stencilTestEnable);
	depthStencilInfo.front = {};
	depthStencilInfo.back = {};

	VkPipelineRenderingCreateInfo pipeline_create{ VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	pipeline_create.pNext = VK_NULL_HANDLE;
	pipeline_create.colorAttachmentCount = static_cast<uint32_t>(details.colorAttachmentFormats.size());
	pipeline_create.pColorAttachmentFormats = details.colorAttachmentFormats.data();
	pipeline_create.depthAttachmentFormat = details.depthAttachmentFormat;
	pipeline_create.stencilAttachmentFormat = details.stencilAttachmentFormat;

	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates = dynamicStates.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &pipeline_create;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = pipelineShaders.getShaderStageInfos();
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	pipelineInfo.pVertexInputState = &vertInputInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pRasterizationState = &rasterInfo;
	pipelineInfo.pMultisampleState = &multInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.layout = newLayout;
	pipelineInfo.renderPass = VK_NULL_HANDLE;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	// Create the actual pipeline
	VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(logDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS)
		throw std::runtime_error("Pipeline layout creation failed");

	// Add the pipeline to the map
	this->pipelines.push_back(compiledPipeline);
	compiledPipeline.vertexAttributes = pipelineShaders.getAttributes();
	compiledPipeline.pipeline = newPipeline;
	compiledPipeline.descriptorSetLayouts = descLayouts;
	compiledPipeline.pushConstantRanges = pipelineShaders.getPushConstantRanges();
	return compiledPipeline;
}

std::optional<CompiledPipelineData> PipelineManager::findPipeline(VertexAttributeFlags attributes)
{
	auto it = std::find_if(pipelines.begin(), pipelines.end(), [attributes](const auto& pipelineData)
		{
			return pipelineData.vertexAttributes == attributes;
		});
	if (it != pipelines.end())
	{
		return *it;
	}
	return std::nullopt;
}
