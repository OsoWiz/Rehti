#include "ShaderTools.hpp"

#include <DescriptorBuilder.hpp>
#include <Logger.hpp>


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

Rehti::Format mapToRehtiFormat(const SpvReflectFormat format)
{
	switch (format)
	{
		case SPV_REFLECT_FORMAT_R32_UINT:
			return Rehti::Format::UInt32;
		case SPV_REFLECT_FORMAT_R32_SINT:
			return Rehti::Format::Int32;
		case SPV_REFLECT_FORMAT_R32_SFLOAT:
			return Rehti::Format::Float32;
		case SPV_REFLECT_FORMAT_R32G32_UINT:
			return Rehti::Format::UVec2;
		case SPV_REFLECT_FORMAT_R32G32_SINT:
			return Rehti::Format::IVec2;
		case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
			return Rehti::Format::Vec2;
		case SPV_REFLECT_FORMAT_R32G32B32_UINT:
			return Rehti::Format::UVec3;
		case SPV_REFLECT_FORMAT_R32G32B32_SINT:
			return Rehti::Format::IVec3;
		case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
			return Rehti::Format::Vec3;
		case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
			return Rehti::Format::UVec4;
		case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
			return Rehti::Format::IVec4;
		case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
			return Rehti::Format::Vec4;
		default:
			Logger::error("Unsupported SPIRV format: " + std::to_string(format));
			return Rehti::Format::Undefined;
	}
}

VkShaderStageFlagBits shadercToVulkanShaderStage(SpvReflectShaderStageFlagBits stage)
{
	switch (stage)
	{
		case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		default:
			return static_cast<VkShaderStageFlagBits>(0);
	}
}

ShaderStageInternal getShaderTypeFromFileExtension(const std::filesystem::path& filePath)
{
	std::string extension = filePath.extension().string();
	if (extension == ".vert")
	{
		return ShaderStageInternal::vertex();
	}
	else if (extension == ".frag")
	{
		return ShaderStageInternal::fragment();
	}
	else if (extension == ".geom")
	{
		return ShaderStageInternal::geometry();
	}
	else if (extension == ".tesc")
	{
		return ShaderStageInternal::tessellation_control();
	}
	else if (extension == ".tese")
	{
		return ShaderStageInternal::tessellation_evaluation();
	}
	else if (extension == ".comp")
	{
		return ShaderStageInternal::compute();
	}
	else
	{
		Logger::error("Unsupported file extension: " + extension);
	}
	return ShaderStageInternal::unknown();
}

int readSpvToShaderData(const std::filesystem::path& filePath, CompiledShaderData& shaderData) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		Logger::error("Failed to open file: " + filePath.string());
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
std::string chooseSourceCode(std::set<VertexAttributeFlags> attributes, std::string& vertexSource, std::string& fragmentSource)
{
	// todo intelligent choosing of source code based on attributes
	assert(false);
	return "";
}

ShaderTools::ShaderTools(VkDevice device)
: device(device)
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
		Logger::error("Failed to reflect shader module!");
	}
	shaderModule.stageFlag = shadercToVulkanShaderStage(module.shader_stage);

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
			Logger::error("Error: binding count mismatch in shader " + std::string(module.source_file) + ":\n" +
				"expected " + std::to_string(set->binding_count) + " got " + std::to_string(goalBindings.size()));
		}
		createInfo.bindingCount = goalBindings.size();
		createInfo.pBindings = goalBindings.data();
		if (setIndex < MAX_DESCRIPTOR_SETS)
		{
			shaderModule.descriptorSetLayouts[set->set] = this->pDescriptorBuilder->createDescriptorSetLayout(createInfo);
			setIndex++;
		}
		else
		{
			Logger::warning("Error: too many descriptor sets in shader " + std::string(module.source_file));
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
	for (auto& input : inputVariables)
	{
		ShaderInterface::ShaderInputOutput inputVar{};
		inputVar.format = mapToRehtiFormat(input->format);
		inputVar.location = input->location;
		shaderModule.interface.inputs.push_back(inputVar);
	}

	// output variables
	std::vector<ShaderInterface> outputAttributes;
	outputAttributes.reserve(outputVariables.size());
	for (auto& output : outputVariables)
	{
		ShaderInterface::ShaderInputOutput outputVar{};
		outputVar.format = mapToRehtiFormat(output->format);
		outputVar.location = output->location;
		shaderModule.interface.outputs.push_back(outputVar);
	}

	// cleanup
	spvReflectDestroyShaderModule(&module);
}

int ShaderTools::compileShader(const std::string& code, const std::string& shaderName, const ShaderStageInternal type, CompiledShaderData& shaderModule)
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
		ShaderStageInternal type = getShaderTypeFromFileExtension(path);
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
	this->compiledShaders.push_back(data);
}

CompiledShaderData ShaderTools::compileShader(const ShaderAsset& shader)
{
	CompiledShaderData data{};
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	shaderc::SpvCompilationResult compResult{};
	switch (shader.format)
	{
		case ShaderAsset::Format::SPIRV:
			data.code = std::vector<uint32_t>(shader.bytes.size() / sizeof(uint32_t));
			std::memcpy(data.code.data(), shader.bytes.data(), shader.bytes.size());
			return data;
		case ShaderAsset::Format::GLSL:
			compResult = compiler.CompileGlslToSpv(reinterpret_cast<const char*>(shader.bytes.data()), shader.bytes.size(), shaderc_shader_kind::shaderc_glsl_infer_from_source, "shader.glsl", options);
			if (compResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				Logger::error(compResult.GetErrorMessage());
				return {};
			}
			data.code = std::vector<uint32_t>(compResult.cbegin(), compResult.cend());
			break;
		default:
			Logger::error("Unsupported shader format!");
			return {};
	}
	
	if (shader.format != ShaderAsset::Format::SPIRV 
		&& compResult.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		Logger::error(compResult.GetErrorMessage());
		return {};
	}

	SpvReflectShaderModule reflectModule{};
	reflectShaderCode(data.code.data(), data.code.size(), reflectModule, data);

	VkShaderModuleCreateInfo vertInfo{};
	vertInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertInfo.codeSize = compResult.cend() - compResult.cbegin();
	vertInfo.pCode = reinterpret_cast<const uint32_t*>(compResult.cbegin());

	if (vkCreateShaderModule(device, &vertInfo, nullptr, &data.module) != VK_SUCCESS)
	{
		Logger::error("Failed to create a vertex shader module!");
		return {};
	}
	compiledShaders.push_back(data);
	return data;
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

VertexAttributeFlags CompiledShaderData::getInputAttributeFlags() const
{
	return interface.getLikelyVertexAttributes();
}
