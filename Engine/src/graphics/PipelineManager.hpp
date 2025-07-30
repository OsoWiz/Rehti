#pragma once

#include <ShaderTools.hpp>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <optional>

struct PipelineShaderInfo
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
	 * @brief Returns the stride of the vertex for vertex shader.
	 * @returns the combined calculated size of the input variables of the vertex shader.
	 */
	uint32_t getStride() const;

	/**
	 * @brief Returns the vertex attribute flags for this shader.
	 * @return VertexAttributeFlags
	 */
	VertexAttributeFlags getAttributes() const;

}; // END OF PipelineShaderInfo

/**
 * @brief Class for holding and managing pipelines
 */
class PipelineManager
{
public:

	PipelineManager(VkDevice& logDevice, VkExtent2D& currentExtent);

	/**
	 * @brief Creates a basic pipeline with the given vertex and fragment shaders.
	 * @param renderPass to associate with the pipeline
	 * @param vShaderData compiled vertex shader
	 * @param fShaderData compiled fragment shader
	 */
	void createBasicPipeline(const VkRenderPass& renderPass, const CompiledShaderData& vShaderData, const CompiledShaderData& fShaderData);


	void createPipeline(const VkRenderPass& renderPass, const PipelineShaderInfo& compiledShaders);

	VkPipeline getPipeline(VertexAttributeFlags attributes);

private:
	// map of vertex attributes to pipelines
	std::unordered_map<VertexAttributeFlags, VkPipeline> pipelines;
	VkDevice logDevice;
	VkExtent2D swapChainExtent;
};