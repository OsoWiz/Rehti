#pragma once

#include <vulkan/vulkan.h>

#include <set>
#include "Vertex.h"

namespace RehtiGraphics
{

	int createShaderModules(const VkDevice& device, std::set<VertexAttribute> attributes, VkShaderModule* vertShaderModule, VkShaderModule* fragShaderModule);

	int createPipelineShaderInfo(const VkDevice& device, std::set<VertexAttribute> attributes, VkPipelineShaderStageCreateInfo& vertShaderStageInfo, VkPipelineShaderStageCreateInfo& fragShaderStageInfo);
}