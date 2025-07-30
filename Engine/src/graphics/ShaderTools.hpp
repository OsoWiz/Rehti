#pragma once

#include <Vertex.hpp>
#include <shaderc/shaderc.hpp>
#include <unordered_map>
#include <array>

// Forward declarations
class DescriptorBuilder;


/**
 * @brief ShaderInterfaceVariable is a pair of VertexAttributeEnum and VkFormat.
 */
using ShaderInterfaceVariable = std::pair<VertexAttributeEnum, VkFormat>;

// forward declarations
class SpvReflectShaderModule;

// constants
constexpr uint32_t MAX_DESCRIPTOR_SETS = 4;

struct ShaderType
{
	shaderc_shader_kind shaderKind;
	VkShaderStageFlags stageFlag;
	const static ShaderType vertex() { return { shaderc_shader_kind::shaderc_vertex_shader, VK_SHADER_STAGE_VERTEX_BIT }; };
	const static ShaderType fragment() { return { shaderc_shader_kind::shaderc_fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT }; };
	const static ShaderType geometry() { return { shaderc_shader_kind::shaderc_geometry_shader, VK_SHADER_STAGE_GEOMETRY_BIT }; };
	const static ShaderType tessellation_control() { return { shaderc_shader_kind::shaderc_tess_control_shader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT }; };
	const static ShaderType tessellation_evaluation() { return { shaderc_shader_kind::shaderc_tess_evaluation_shader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT }; };
	const static ShaderType compute() { return { shaderc_shader_kind::shaderc_compute_shader, VK_SHADER_STAGE_COMPUTE_BIT }; };
	const static ShaderType unknown() { return { shaderc_shader_kind::shaderc_glsl_infer_from_source, VK_SHADER_STAGE_ALL }; };
};

struct CompiledShaderData
{
	std::vector<VkPushConstantRange> pushConstantRanges;
	std::array<VkDescriptorSetLayout, MAX_DESCRIPTOR_SETS> descriptorSetLayouts;
	std::vector<ShaderInterfaceVariable> inputAttributes;
	std::vector<ShaderInterfaceVariable> outputAttributes;
	std::vector<uint32_t> code;
	VkShaderModule module;
	VkShaderStageFlagBits stageFlag;

	VkPipelineShaderStageCreateInfo getShaderStageInfo() const;
};

class ShaderTools
{

public:
	ShaderTools(VkDevice device);
	~ShaderTools();

	/**
	 * @brief
	 * @param shaderPath is the path to the shader file.
	 */
	void loadShader(const std::string& shaderPath);

	/**
	 * @brief clears all the compiled shaders.
	 */
	void clear();

private:
	/**
	 * @brief Reflects the given spirv shader code
	 * @param pCode
	 * @param codeSize
	 * @param module
	 * @param shaderModule
	 */
	void reflectShaderCode(const uint32_t* pCode, const size_t codeSize, SpvReflectShaderModule& module, CompiledShaderData& shaderModule);
	int compileShader(const std::string& code, const std::string& shaderName, const ShaderType type, CompiledShaderData& shaderModule);
	bool validate(const CompiledShaderData& shaderModule);
	VkDevice device;
	std::unique_ptr<DescriptorBuilder> pDescriptorBuilder;
	std::unordered_map<std::string, CompiledShaderData> compiledShaders;
};