#include "ShaderTools.hpp"

#include <Vertex.hpp>

#include <DescriptorBuilder.hpp>

// 3rd party
#include <spirv-reflect/spirv_reflect.h>

// stl
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <set>
#include <array>
#include <utility>


int createShaderModules(const VkDevice& device, std::set<VertexAttributeEnum> attributes, VkShaderModule* vertShaderModule, VkShaderModule* fragShaderModule);

int createPipelineShaderInfo(const VkDevice& device, std::set<VertexAttributeEnum> attributes, VkPipelineShaderStageCreateInfo& vertShaderStageInfo, VkPipelineShaderStageCreateInfo& fragShaderStageInfo);


/**
 * @brief Checks whether file is already compiled based on the extension.
 * @param filePath
 * @return
 */
bool isCompiled(const std::filesystem::path& filePath)
{
	std::string extension = filePath.extension().string();
	return extension == ".spv";
}

ShaderType getShaderTypeFromFileExtension(const std::filesystem::path& filePath)
{
	std::string extension = filePath.extension().string();
	if (extension == ".vert")
	{
		return ShaderType::vertex();
	}
	else if (extension == ".frag")
	{
		return ShaderType::fragment();
	}
	else if (extension == ".geom")
	{
		return ShaderType::geometry();
	}
	else if (extension == ".tesc")
	{
		return ShaderType::tessellation_control();
	}
	else if (extension == ".tese")
	{
		return ShaderType::tessellation_evaluation();
	}
	else if (extension == ".comp")
	{
		return ShaderType::compute();
	}
	else
	{
		std::cerr << " Unsupported file extension: " << extension << std::endl;
	}
	return ShaderType::unknown();
}

int readSpvToShaderData(const std::filesystem::path& filePath, CompiledShaderData& shaderData) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << filePath.string() << std::endl;
		return 1;
	}
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint32_t> code(fileSize / sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(code.data()), fileSize);

	file.close();
	return 0;
}

// does nothing currently
std::string chooseSourceCode(std::set<VertexAttributeEnum> attributes, std::string& vertexSource, std::string& fragmentSource)
{
	// todo intelligent choosing of source code based on attributes
	assert(false);
	return "";
}

int createShaderModules(const VkDevice& device, std::set<VertexAttributeEnum> attributes, VkShaderModule* vertShaderModule, VkShaderModule* fragShaderModule)
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

int createPipelineShaderInfo(const VkDevice& device, std::set<VertexAttributeEnum> attributes, VkPipelineShaderStageCreateInfo& vertShaderStageInfo, VkPipelineShaderStageCreateInfo& fragShaderStageInfo)
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

ShaderTools::ShaderTools(VkDevice device)
{

	this->pDescriptorBuilder = std::make_unique<DescriptorBuilder>(device);
}

ShaderTools::~ShaderTools()
{
}

void ShaderTools::reflectShaderCode(const uint32_t* pCode, const size_t codeSize, SpvReflectShaderModule& module, CompiledShaderData& shaderModule)
{
	SpvReflectResult reflectionRes = spvReflectCreateShaderModule(codeSize, pCode, &module);
	if (reflectionRes != SPV_REFLECT_RESULT_SUCCESS)
	{
		std::cerr << "Failed to reflect shader module!" << std::endl;
	}
	shaderModule.stageFlag = static_cast<VkShaderStageFlagBits>(module.shader_stage);

	uint32_t count = 0; // count for each reflectable variable.

	// desc sets
	spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
	std::vector<SpvReflectDescriptorSet*> sets(count);
	spvReflectEnumerateDescriptorSets(&module, &count, sets.data());

	// desc bindings
	spvReflectEnumerateDescriptorBindings(&module, &count, nullptr);
	std::vector<SpvReflectDescriptorBinding*> reflectedBindings(count);
	spvReflectEnumerateDescriptorBindings(&module, &count, reflectedBindings.data());

	// push constants
	spvReflectEnumeratePushConstantBlocks(&module, &count, nullptr);
	std::vector<SpvReflectBlockVariable*> pushConstants(count);
	spvReflectEnumeratePushConstantBlocks(&module, &count, pushConstants.data());

	// input variables
	spvReflectEnumerateInputVariables(&module, &count, nullptr);
	std::vector<SpvReflectInterfaceVariable*> inputVariables(count);
	spvReflectEnumerateInputVariables(&module, &count, inputVariables.data());

	// output variables
	spvReflectEnumerateOutputVariables(&module, &count, nullptr);
	std::vector<SpvReflectInterfaceVariable*> outputVariables(count);
	spvReflectEnumerateOutputVariables(&module, &count, outputVariables.data());

	// convert to our own data structures
	uint32_t setIndex = 0;
	for (auto& set : sets)
	{
		VkDescriptorSetLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = set->binding_count;
		createInfo.flags = 0;
		std::vector<VkDescriptorSetLayoutBinding> goalBindings;
		for (auto& binding : reflectedBindings)
		{
			if (binding->set == set->set)
			{
				VkDescriptorSetLayoutBinding layoutBinding{};
				layoutBinding.binding = binding->binding;
				layoutBinding.descriptorType = static_cast<VkDescriptorType>(binding->descriptor_type); // should be 1 to 1
				layoutBinding.descriptorCount = binding->count;
				// binding->name; // unused name param
				goalBindings.push_back(layoutBinding);
			}
		}
		if (goalBindings.size() != set->binding_count)
		{
			std::cerr << "Error: binding count mismatch in shader " << module.source_file << ":\n" <<
				"expected " << set->binding_count << " got " << goalBindings.size() << std::endl;
		}
		createInfo.bindingCount = goalBindings.size();
		createInfo.pBindings = goalBindings.data();
		if (setIndex < MAX_DESCRIPTOR_SETS)
		{
			shaderModule.descriptorSetLayouts[setIndex] = this->pDescriptorBuilder->createDescriptorSetLayout(createInfo);
			setIndex++;
		}
		else
		{
			std::cerr << "Error: too many descriptor sets in shader " << module.source_file << std::endl;
		}
	}
	// push constants
	for (auto& pushConstant : pushConstants)
	{
		VkPushConstantRange range{};
		range.offset = pushConstant->offset;
		range.size = pushConstant->size;
		range.stageFlags = static_cast<VkShaderStageFlags>(module.shader_stage);
		shaderModule.pushConstantRanges.push_back(range);
	}

	// input variables
	shaderModule.inputAttributes.reserve(inputVariables.size());
	for (auto& input : inputVariables)
	{
		ShaderInterfaceVariable var{};
		var.second = static_cast<VkFormat>(input->format);
		shaderModule.inputAttributes[input->location] = var;
	}

	// output variables
	shaderModule.outputAttributes.reserve(outputVariables.size());
	for (auto& output : outputVariables)
	{
		ShaderInterfaceVariable var{};
		var.second = static_cast<VkFormat>(output->format);
		shaderModule.outputAttributes[output->location] = var;
	}

	// cleanup
	spvReflectDestroyShaderModule(&module);

}

int ShaderTools::compileShader(const std::string& code, const std::string& shaderName, const ShaderType type, CompiledShaderData& shaderModule)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(code, type.shaderKind, shaderName.c_str(), options);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		std::cerr << result.GetErrorMessage();
		return 1;
	}

	VkShaderModuleCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = result.cend() - result.cbegin();
	shaderModule.code = std::vector(result.cbegin(), result.cend());
	if (vkCreateShaderModule(device, &info, nullptr, &shaderModule.module) != VK_SUCCESS)
	{
		std::cerr << "Failed to create a shader module!" << std::endl;
		return 1;
	}
	return 0;
}

void ShaderTools::loadShader(const std::string& shaderPath)
{
	std::filesystem::path path = shaderPath;
	CompiledShaderData data{};
	if (isCompiled(path))
	{
		if (readSpvToShaderData(path, data) != 0)
		{
			std::cerr << "Failed to read compiled shader: " << path.string() << std::endl;
			return;
		};
	}
	else
	{
		std::ifstream ifs(path);
		ShaderType type = getShaderTypeFromFileExtension(path);
		std::string sourceCode(std::istreambuf_iterator<char>{ifs}, {});
		if (compileShader(sourceCode, path.string(), type, data) != 0)
		{
			std::cerr << "Failed to compile shader: " << path.string() << std::endl;
			return;
		}
	}

	SpvReflectShaderModule reflectModule{};
	reflectShaderCode(data.code.data(), data.code.size(), reflectModule, data);
	// set shader module to the map
	this->compiledShaders[path.string()] = data;
}

bool ShaderTools::validate(const CompiledShaderData& shaderModule)
{
	return shaderModule.module != VK_NULL_HANDLE
		&& shaderModule.stageFlag != 0
		&& shaderModule.code.empty() == false;
}

VkPipelineShaderStageCreateInfo CompiledShaderData::getShaderStageInfo() const
{
	VkPipelineShaderStageCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.stage = stageFlag;
	info.module = module;
	info.pName = "main";
	return info;

}
