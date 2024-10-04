#include "ShaderTools.h"

#include <shaderc/shaderc.hpp>
#include <iostream>

const  std::string testVertShaderCode =
#include "shaders/test.vert"
"";

const std::string testFragShaderCode =
#include "shaders/test.frag"
"";

std::string chooseSourceCode(std::set<VertexAttribute> attributes, std::string& vertexSource, std::string& fragmentSource)
{
	// todo intelligent choosing of source code based on attributes
	vertexSource = testVertShaderCode;
	fragmentSource = testFragShaderCode;
}

int RehtiGraphics::createShaderModules(const VkDevice& device, std::set<VertexAttribute> attributes, VkShaderModule* vertShaderModule, VkShaderModule* fragShaderModule)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	std::string vertexSource;
	std::string fragmentSource;
	chooseSourceCode(attributes, vertexSource, fragmentSource);

	shaderc::SpvCompilationResult vertResult = compiler.CompileGlslToSpv(vertexSource, shaderc_shader_kind::shaderc_vertex_shader, "test.vert", options);
	if (vertResult.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << vertResult.GetErrorMessage();
		return 0;
	}

	VkShaderModuleCreateInfo vertInfo{};
	vertInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertInfo.codeSize = vertResult.cend() - vertResult.cbegin();
	vertInfo.pCode = reinterpret_cast<const uint32_t*>(vertResult.cbegin());

	if (vkCreateShaderModule(device, &vertInfo, nullptr, vertShaderModule) != VK_SUCCESS)
	{
		std::cerr << "Failed to create a vertex shader module!" << std::endl;
		return 0;
	}

	shaderc::SpvCompilationResult fragResult = compiler.CompileGlslToSpv(fragmentSource, shaderc_shader_kind::shaderc_fragment_shader, "test.frag", options);
	if (fragResult.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << fragResult.GetErrorMessage();
		return 0;
	}

	VkShaderModuleCreateInfo fragInfo{};
	fragInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragInfo.codeSize = fragResult.cend() - fragResult.cbegin();
	fragInfo.pCode = reinterpret_cast<const uint32_t*>(fragResult.cbegin());

	if (vkCreateShaderModule(device, &fragInfo, nullptr, fragShaderModule) != VK_SUCCESS)
	{
		std::cerr << "Failed to create a fragment shader module!" << std::endl;
		return 0;
	}

	return 1;
}

int RehtiGraphics::createPipelineShaderInfo(const VkDevice& device, std::set<VertexAttribute> attributes, VkPipelineShaderStageCreateInfo& vertShaderStageInfo, VkPipelineShaderStageCreateInfo& fragShaderStageInfo)
{
	VkPipelineShaderStageCreateInfo vertShaderInfo{};
	vertShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderInfo{};
	fragShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderInfo.pName = "main";

	createShaderModules(device, attributes, &vertShaderInfo.module, &fragShaderInfo.module);

	vertShaderStageInfo = vertShaderInfo;
	fragShaderStageInfo = fragShaderInfo;

	return 1;
}
