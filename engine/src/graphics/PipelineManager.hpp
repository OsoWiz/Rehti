#pragma once

#include <ShaderTools.hpp>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <optional>

// FWD
struct GraphicsPipelineConfig;

struct PipelineShaderData
{
	std::optional<CompiledShaderData> vertexShaderData;
	std::optional<CompiledShaderData> tessControlShaderData;
	std::optional<CompiledShaderData> tessEvalShaderData;
	std::optional<CompiledShaderData> geometryShaderData;
	std::optional<CompiledShaderData> fragmentShaderData;

	bool isComplete() const
	{
		return vertexShaderData.has_value() && fragmentShaderData.has_value();
	}

	bool hasTessellation() const
	{
		return tessControlShaderData.has_value() && tessEvalShaderData.has_value();
	}

	/**
	 * @brief Return the vertex input attributes for the given binding.
	 * @param binding is the binding to set for attributes.
	 * @return vector containing the reflected input variables for this shader. NOTE: this function should only be called for vertex shaders.
	 */
	std::vector<VkVertexInputAttributeDescription> getVertexAttributes(uint32_t binding = 0) const;

	std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const;

	std::vector<VkPushConstantRange> getPushConstantRanges() const;

	std::vector<VkPipelineShaderStageCreateInfo> getShaderStageInfos() const;

	/**
	 * @brief Returns the stride of the vertex ASSUMING vertex attributes are interleaved.
	 * @returns the combined calculated size of the input variables of the vertex shader.
	 */
	size_t getStride() const;

	/**
	 * @brief Returns the vertex attribute flags for this shader.
	 * @return VertexAttributeFlags
	 */
	VertexAttributeFlags getAttributes() const;

}; // END OF PipelineShaderData

struct PipelineCreationDetails
{
	bool interleavedVertexData = false;
	std::vector<VkFormat> colorAttachmentFormats;
	VkFormat depthAttachmentFormat;
	VkFormat stencilAttachmentFormat;
};


struct CompiledPipelineData
{
	VkPipeline pipeline;
	VkPipelineLayout layout;
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	std::vector<VkPushConstantRange> pushConstantRanges;
	VertexAttributeFlags vertexAttributes;
};

/**
 * @brief Class for holding and managing pipelines
 */
class PipelineManager
{
public:

	PipelineManager(VkDevice& logDevice);
	~PipelineManager();

	CompiledPipelineData createPipeline(const GraphicsPipelineConfig& config, const PipelineCreationDetails& details);

	std::optional<CompiledPipelineData> findPipeline(VertexAttributeFlags attributes);

	std::vector<VkDynamicState> getDynamicStates() const;
private:
	PipelineShaderData getPipelineShaders(const GraphicsPipelineConfig& config);
	std::vector<CompiledPipelineData> pipelines;
	VkDevice logDevice;
	std::unique_ptr<ShaderTools> shaderTools;
	// std::vector<VkDynamicState> dynamicStates; TODO add ability to configure dynamic states. (ctor?)
};