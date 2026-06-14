#include "GraphicsUtils.hpp"

VkFormat getFormatFromEnum(VertexAttributeFlags attribute)
{
	switch (attribute)
	{
		case VertexAttributeFlags::POSITION:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case VertexAttributeFlags::NORMAL:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case VertexAttributeFlags::COLOR:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case VertexAttributeFlags::TEXCOORD:
			return VK_FORMAT_R32G32_SFLOAT;
		case VertexAttributeFlags::TANGENT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case VertexAttributeFlags::JOINTS:
			return VK_FORMAT_R32G32B32A32_UINT;
		case VertexAttributeFlags::WEIGHTS:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	return VK_FORMAT_UNDEFINED;
}

uint32_t attributeEnumToSize(VertexAttributeFlags venum)
{
	switch (venum)
	{
		case VertexAttributeFlags::POSITION:
			return 3;
		case VertexAttributeFlags::NORMAL:
			return 3;
		case VertexAttributeFlags::COLOR:
			return 4;
		case VertexAttributeFlags::TEXCOORD:
			return 2;
		case VertexAttributeFlags::TANGENT:
			return 3;
		case VertexAttributeFlags::JOINTS:
			return 4;
		case VertexAttributeFlags::WEIGHTS:
			return 4;
		default:
			return 0;
	}
}

VertexAttributeInfo getAttributeInfo(VkFormat format)
{
	VertexAttributeInfo info{ format, 0 };
	switch (format)
	{
		case VK_FORMAT_R32G32B32_SFLOAT:
			info.size = 3 * sizeof(float);
			break;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			info.size = 4 * sizeof(float);
			break;
		case VK_FORMAT_R32G32_SFLOAT:
			info.size = 2 * sizeof(float);
			break;
		case VK_FORMAT_R32G32B32A32_UINT:
			info.size = 4 * sizeof(uint32_t);
			break;
		case VK_FORMAT_R32G32B32_UINT:
			info.size = 3 * sizeof(uint32_t);
			break;
		default:
			info.size = 0;
			break;
	}
	return info;
}

VertexAttributeInfo getAttributeInfo(VertexAttributeFlags attribute)
{
	switch (attribute)
	{
		case VertexAttributeFlags::POSITION:
			return { VK_FORMAT_R32G32B32_SFLOAT, VertexAttributes::Position::getSize() };
		case VertexAttributeFlags::NORMAL:
			return { VK_FORMAT_R32G32B32_SFLOAT, VertexAttributes::Normal::getSize() };
		case VertexAttributeFlags::COLOR:
			return { VK_FORMAT_R32G32B32A32_SFLOAT, VertexAttributes::Color::getSize() };
		case VertexAttributeFlags::TEXCOORD:
			return { VK_FORMAT_R32G32_SFLOAT, VertexAttributes::TexCoord::getSize() };
		case VertexAttributeFlags::TANGENT:
			return { VK_FORMAT_R32G32B32_SFLOAT, VertexAttributes::Tangent::getSize() };
		case VertexAttributeFlags::JOINTS:
			return { VK_FORMAT_R32G32B32A32_UINT, VertexAttributes::Joints::getSize() };
		case VertexAttributeFlags::WEIGHTS:
			return { VK_FORMAT_R32G32B32A32_SFLOAT, VertexAttributes::Weights::getSize() };
		default:
			return { VK_FORMAT_UNDEFINED, 0 };
	}
}

std::vector<PlanarVertexInputInfo> getPlanarVertexInputInfo(const Mesh& mesh)
{
	std::vector<PlanarVertexInputInfo> info{};
	VertexAttributeFlags attributes = mesh.getAvailableVertexAttributes();
	using UnderlyingType = std::underlying_type_t<VertexAttributeFlags>;
	uint32_t binding = 0;
	for (VertexAttributeFlags attr = VertexAttributeFlags::POSITION; attr < VertexAttributeFlags::UNDEFINED; attr <<= VertexAttributeFlags::POSITION)
	{
		if (hasFlag(attributes, attr))
		{
			VertexAttributeInfo attributeInfo = getAttributeInfo(static_cast<VertexAttributeFlags>(attr));
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = binding;
			bindingDescription.stride = attributeInfo.size;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			VkVertexInputAttributeDescription attributeDescription{};
			attributeDescription.location = binding;
			attributeDescription.binding = bindingDescription.binding;
			attributeDescription.format = attributeInfo.format;
			attributeDescription.offset = 0u;
			info.emplace_back(bindingDescription, attributeDescription);
			binding++;
		}
	}

	return info;
}

uint32_t calculateOffset(VertexAttributeFlags attribute)
{
	uint32_t offset = 0;

	for (uint16_t e = static_cast<std::underlying_type_t<VertexAttributeFlags>>(VertexAttributeFlags::POSITION); e < (uint16_t)attribute; e++)
	{
		offset += attributeEnumToSize((VertexAttributeFlags)e);
	}
	return offset;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physDevice, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies.data());
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);
		if (presentSupport)
		{
			indices.presentFamily = i;
		}
		if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			indices.transferFamily = i;
		}
		if (indices.hasAll() || indices.isComplete()) // todo make this configurable
		{
			break;
		}
		i++;
	}
	return indices;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}



VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

size_t calculateStride(const std::vector<ShaderInterface::ShaderInputOutput>& inputs)
{
	size_t stride = 0;
	for (const auto& input : inputs)
	{
		stride += Rehti::formatSize(input.format);
	}
	return stride;
}

std::vector<uint8_t> Mapping::toBytes(const Mesh& mesh)
{
	std::vector<uint8_t>blob{};
	blob.reserve(mesh.getSize());
	VertexAttributeFlags flags = mesh.getAvailableVertexAttributes();
	uint8_t* dst = blob.data();
	if (!mesh.positions.empty())
	{
		size_t copyRegionSize = mesh.positions.size() * VertexAttributes::Position::getSize();
		std::memcpy(dst, mesh.positions.data(), copyRegionSize);
		dst += copyRegionSize;
	}
	if (!mesh.normals.empty())
	{
		size_t copyRegionSize = mesh.normals.size() * VertexAttributes::Normal::getSize();
		std::memcpy(dst, mesh.normals.data(), copyRegionSize);
		dst += copyRegionSize;
	}
	if (!mesh.colors.empty())
	{
		size_t copyRegionSize = mesh.colors.size() * VertexAttributes::Color::getSize();
		std::memcpy(dst, mesh.colors.data(), copyRegionSize);
		dst += copyRegionSize;
	}
	if (!mesh.texCoords.empty())
	{
		size_t copyRegionSize = mesh.texCoords.size() * VertexAttributes::TexCoord::getSize();
		std::memcpy(dst, mesh.texCoords.data(), copyRegionSize);
		dst += copyRegionSize;
	}
	if (!mesh.tangents.empty())
	{
		size_t copyRegionSize = mesh.tangents.size() * VertexAttributes::Tangent::getSize();
		std::memcpy(dst, mesh.tangents.data(), copyRegionSize);
		dst += copyRegionSize;
	}
	if (!mesh.bitangents.empty())
	{
		size_t copyRegionSize = mesh.bitangents.size() * VertexAttributes::Bitangent::getSize();
		std::memcpy(dst, mesh.bitangents.data(), copyRegionSize);
		dst += copyRegionSize;
	}
	if (!mesh.joints.empty())
	{
		size_t copyRegionSize = mesh.joints.size() * VertexAttributes::Joints::getSize();
		std::memcpy(dst, mesh.joints.data(), copyRegionSize);
		dst += copyRegionSize;
	}
	if (!mesh.weights.empty())
	{
		size_t copyRegionSize = mesh.weights.size() * VertexAttributes::Weights::getSize();
		std::memcpy(dst, mesh.weights.data(), copyRegionSize);
		dst += copyRegionSize;
	}

	return blob;
}

VkCullModeFlagBits Mapping::toVkType(RasterizationConfig::CullMode mode)
{
	switch (mode)
	{
		case RasterizationConfig::CullMode::NONE:
			return VK_CULL_MODE_NONE;
		case RasterizationConfig::CullMode::FRONT:
			return VK_CULL_MODE_FRONT_BIT;
		case RasterizationConfig::CullMode::BACK:
			return VK_CULL_MODE_BACK_BIT;
		case RasterizationConfig::CullMode::FRONT_AND_BACK:
			return VK_CULL_MODE_FRONT_AND_BACK;
		case RasterizationConfig::CullMode::DYNAMIC:
			return VK_CULL_MODE_NONE;
	}

	return VkCullModeFlagBits();
}

VkPolygonMode Mapping::toVkType(RasterizationConfig::PolygonFillMode mode)
{
	switch (mode)
	{
		case RasterizationConfig::PolygonFillMode::FILL:
			return VK_POLYGON_MODE_FILL;
		case RasterizationConfig::PolygonFillMode::LINE:
			return VK_POLYGON_MODE_LINE;
		case RasterizationConfig::PolygonFillMode::POINT:
			return VK_POLYGON_MODE_POINT;
		case RasterizationConfig::PolygonFillMode::DYNAMIC:
			return VK_POLYGON_MODE_MAX_ENUM;
	}
	return VkPolygonMode();
}

VkFrontFace Mapping::toVkType(RasterizationConfig::WindingOrder mode)
{
	switch (mode)
	{
		case RasterizationConfig::WindingOrder::CLOCKWISE:
			return VK_FRONT_FACE_CLOCKWISE;
		case RasterizationConfig::WindingOrder::COUNTER_CLOCKWISE:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	}
	return VkFrontFace();
}

VkFormat Mapping::toVkFormat(Rehti::Format format)
{
	switch (format)
	{
		case Rehti::Format::Int8:
			return VK_FORMAT_R8_SINT;
		case Rehti::Format::UInt8:
			return VK_FORMAT_R8_UINT;
		case Rehti::Format::Int16:
			return VK_FORMAT_R16_SINT;
		case Rehti::Format::UInt16:
			return VK_FORMAT_R16_UINT;
		case Rehti::Format::Int32:
			return VK_FORMAT_R32_SINT;
		case Rehti::Format::UInt32:
			return VK_FORMAT_R32_UINT;
		case Rehti::Format::Int64:
			return VK_FORMAT_R64_SINT;
		case Rehti::Format::UInt64:
			return VK_FORMAT_R64_UINT;
		case Rehti::Format::Float32:
			return VK_FORMAT_R32_SFLOAT;
		case Rehti::Format::Float64:
			return VK_FORMAT_R64_SFLOAT;
		case Rehti::Format::Vec2:
			return VK_FORMAT_R32G32_SFLOAT;
		case Rehti::Format::Vec3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case Rehti::Format::Vec4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Rehti::Format::IVec2:
			return VK_FORMAT_R32G32_SINT;
		case Rehti::Format::IVec3:
			return VK_FORMAT_R32G32B32_SINT;
		case Rehti::Format::IVec4:
			return VK_FORMAT_R32G32B32A32_SINT;
		case Rehti::Format::UVec2:
			return VK_FORMAT_R32G32_UINT;
		case Rehti::Format::UVec3:
			return VK_FORMAT_R32G32B32_UINT;
		case Rehti::Format::UVec4:
			return VK_FORMAT_R32G32B32A32_UINT;
		case Rehti::Format::DVec2:
			return VK_FORMAT_R64G64_SFLOAT;
		case Rehti::Format::DVec3:
			return VK_FORMAT_R64G64B64_SFLOAT;
		case Rehti::Format::DVec4:
			return VK_FORMAT_R64G64B64A64_SFLOAT;
		case Rehti::Format::Mat2:
		case Rehti::Format::Mat3:
		case Rehti::Format::Mat4:
		case Rehti::Format::DMat2:
		case Rehti::Format::DMat3:
		case Rehti::Format::DMat4:
		case Rehti::Format::Undefined:
		default:
			return VK_FORMAT_UNDEFINED;
	}
}

std::vector<VkVertexInputAttributeDescription> Mapping::getVertexAttributeDescriptions(const ShaderInterface& shader, uint32_t binding)
{
    std::vector<VkVertexInputAttributeDescription> descriptions;

	for (uint32_t i = 0; i < shader.inputs.size(); ++i)
	{
		const auto& input = shader.inputs[i];
		VkVertexInputAttributeDescription desc{};
		desc.binding = binding;
		desc.location = input.location;
		desc.format = toVkFormat(input.format);
		desc.offset = 0;
		descriptions.push_back(desc);
	}

	return descriptions;
}

std::vector<VkVertexInputBindingDescription> Mapping::getVertexBindingDescriptions(const ShaderInterface& shader, bool interleaved)
{
    std::vector<VkVertexInputBindingDescription> descriptions;

	if (interleaved)
	{
		if (shader.inputs.empty())
			return descriptions;

		VkVertexInputBindingDescription desc{};
		desc.binding = 0;
		desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		desc.stride = 0;

		for (const auto& input : shader.inputs)
		{
			desc.stride += static_cast<uint32_t>(Rehti::formatSize(input.format));
		}

		descriptions.push_back(desc);
	}
	else
	{
		for (uint32_t i = 0; i < shader.inputs.size(); ++i)
		{
			const auto& input = shader.inputs[i];
			VkVertexInputBindingDescription desc{};
			desc.binding = i;
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			desc.stride = static_cast<uint32_t>(Rehti::formatSize(input.format));
			descriptions.push_back(desc);
		}
	}

	return descriptions;
}
