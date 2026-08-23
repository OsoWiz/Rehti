#pragma once
#include "GraphicsTypes.hpp"
#include "Logger.hpp"
#include <vulkan/vulkan.h>
#include <tuple>
#include <optional>

// helper structs
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> transferFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

	bool hasAll()
	{
		return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
	}
};


struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct VertexAttributeInfo
{
	VkFormat format;
	size_t size;
};

std::vector<VertexAttributeInfo> getAttributeInfos(VertexAttributeFlags attributes);

VertexAttributeInfo getAttributeInfo(VkFormat format);

using PlanarVertexInputInfo = std::pair<VkVertexInputBindingDescription, VkVertexInputAttributeDescription>;
std::vector<PlanarVertexInputInfo> getPlanarVertexInputInfo(const Mesh& mesh);

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice, VkSurfaceKHR surface);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physDevice, VkSurfaceKHR surface);
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

size_t calculateStride(const std::vector<ShaderInterface::ShaderInputOutput>& inputs);

// checks whether the first parameter is the superset of the second parameter
bool isSubset(const VertexAttributeFlags& superset, const VertexAttributeFlags& subset);

namespace Mapping
{
	std::vector<uint8_t> toBytes(const Mesh& mesh);

	VkCullModeFlagBits toVkType(RasterizationConfig::CullMode mode);
	VkPolygonMode toVkType(RasterizationConfig::PolygonFillMode mode);
	VkFrontFace toVkType(RasterizationConfig::WindingOrder mode);

	// Maps Rehti::Format to VkFormat for vertex input
	VkFormat toVkFormat(Rehti::Format format);

	// Generates vertex input attribute descriptions from ShaderInterface inputs
	std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions(const ShaderInterface& shader, uint32_t binding = 0);

	// Generates vertex input binding descriptions from ShaderInterface
	// If interleaved is true, all attributes use binding 0 with combined stride
	// If interleaved is false, each attribute gets its own binding with individual stride
	std::vector<VkVertexInputBindingDescription> getVertexBindingDescriptions(const ShaderInterface& shader, bool interleaved = true);
};