#include "PipelineManager.hpp"

std::vector<VkVertexInputAttributeDescription> PipelineShaderInfo::getVertexAttributes(uint32_t binding) const
{
	if (!vertexShaderData.has_value())
	{
		std::cerr << "Error: No vertex shader currently set!" << std::endl;
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

std::vector<VkDescriptorSetLayout> PipelineShaderInfo::getDescriptorSetLayouts() const
{
	std::vector<VkDescriptorSetLayout> layouts;
	auto pushBack = [&layouts](VkDescriptorSetLayout layout) { layouts.push_back(layout); };
	if (vertexShaderData.has_value())
	{
		std::for_each(vertexShaderData->descriptorSetLayouts.begin(), vertexShaderData->descriptorSetLayouts.end(), pushBack);
	}
	if (tessControlShaderData.has_value())
	{
		std::for_each(tessControlShaderData->descriptorSetLayouts.begin(), tessControlShaderData->descriptorSetLayouts.end(), pushBack);
	}
	if (tessEvalShaderData.has_value())
	{
		std::for_each(tessEvalShaderData->descriptorSetLayouts.begin(), tessEvalShaderData->descriptorSetLayouts.end(), pushBack);
	}
	if (geometryShaderData.has_value())
	{
		std::for_each(geometryShaderData->descriptorSetLayouts.begin(), geometryShaderData->descriptorSetLayouts.end(), pushBack);
	}
	if (fragmentShaderData.has_value())
	{
		std::for_each(fragmentShaderData->descriptorSetLayouts.begin(), fragmentShaderData->descriptorSetLayouts.end(), pushBack);
	}

	return layouts;
}

std::vector<VkPushConstantRange> PipelineShaderInfo::getPushConstantRanges() const
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

std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderInfo::getShaderStageInfos() const
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

uint32_t PipelineShaderInfo::getStride() const
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

VertexAttributeFlags PipelineShaderInfo::getAttributes() const
{
	if (!vertexShaderData.has_value())
	{
		std::cerr << "Error: No vertex shader currently set!" << std::endl;
		return {};
	}
	uint16_t flags = 0;
	for (const auto& [attribute, format] : vertexShaderData.value().inputAttributes)
	{
		flags |= static_cast<uint16_t>(1 << attribute);
	}
	return static_cast<VertexAttributeFlags>(flags);
}

PipelineManager::PipelineManager(VkDevice& logDevice, VkExtent2D& currentExtent)
	: logDevice(logDevice), swapChainExtent(currentExtent)
{
}

void PipelineManager::createBasicPipeline(const VkRenderPass& renderPass, const CompiledShaderData& vShaderData, const CompiledShaderData& fShaderData)
{
	PipelineShaderInfo shaderInfo{};
	shaderInfo.vertexShaderData = vShaderData;
	shaderInfo.fragmentShaderData = fShaderData;
	createPipeline(renderPass, shaderInfo);
}

void PipelineManager::createPipeline(const VkRenderPass& renderPass, const PipelineShaderInfo& compiledShaders)
{
	if (!compiledShaders.isComplete())
	{
		std::cerr << "PipelineShaderInfo is missing vertex or fragment shader:\n";
		if (!compiledShaders.vertexShaderData.has_value())
			std::cerr << " Vertex shader missing\n";
		if (!compiledShaders.fragmentShaderData.has_value())
			std::cerr << " Fragment shader missing\n";
		std::cerr << std::endl;
		return;
	}

	VkVertexInputBindingDescription bindingDesc{};
	bindingDesc.binding = 0;
	bindingDesc.stride = compiledShaders.getStride();
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> attributeDescs = compiledShaders.getVertexAttributes(bindingDesc.binding);

	VkPipelineVertexInputStateCreateInfo vertInputInfo{};
	vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInputInfo.vertexBindingDescriptionCount = 1;
	vertInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertInputInfo.vertexAttributeDescriptionCount = attributeDescs.size();
	vertInputInfo.pVertexAttributeDescriptions = attributeDescs.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewPort{};
	viewPort.x = 0.f;
	viewPort.y = 0.f;
	viewPort.width = swapChainExtent.width;
	viewPort.height = swapChainExtent.height;
	viewPort.minDepth = 0.f;
	viewPort.maxDepth = 1.f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportInfo{};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewPort;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterInfo{};
	rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterInfo.depthClampEnable = VK_FALSE;
	rasterInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterInfo.lineWidth = 1.0f;
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // This is because y is flipped in perspective matrix
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
	std::vector<VkDescriptorSetLayout> descLayouts = compiledShaders.getDescriptorSetLayouts();
	pipelinelayoutInfo.setLayoutCount = static_cast<uint32_t>(descLayouts.size());
	pipelinelayoutInfo.pSetLayouts = descLayouts.data();
	std::vector<VkPushConstantRange> pushConstants = compiledShaders.getPushConstantRanges();
	pipelinelayoutInfo.pushConstantRangeCount = pushConstants.size();
	pipelinelayoutInfo.pPushConstantRanges = pushConstants.data();

	// Create pipelinelayout for the given object
	VkPipelineLayout newLayout;
	if (vkCreatePipelineLayout(logDevice, &pipelinelayoutInfo, nullptr, &newLayout))
		throw std::runtime_error("Pipeline layout creation failed");

	VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = VK_TRUE;
	depthStencilInfo.depthWriteEnable = VK_TRUE;
	depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.minDepthBounds = 0.f; // Optional
	depthStencilInfo.maxDepthBounds = 1.f;
	depthStencilInfo.stencilTestEnable = VK_FALSE; // no stencil test
	depthStencilInfo.front = {};                   // Optional
	depthStencilInfo.back = {};

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = compiledShaders.getShaderStageInfos();
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	pipelineInfo.pVertexInputState = &vertInputInfo;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pRasterizationState = &rasterInfo;
	pipelineInfo.pMultisampleState = &multInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.layout = newLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	// Create the actual pipeline
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(logDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS)
		throw std::runtime_error("Pipeline layout creation failed");

	// Add the pipeline to the map
	this->pipelines[compiledShaders.getAttributes()] = newPipeline;
}

VkPipeline PipelineManager::getPipeline(VertexAttributeFlags attributes)
{
	return this->pipelines[attributes];
}
