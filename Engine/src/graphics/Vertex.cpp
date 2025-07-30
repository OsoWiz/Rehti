#include "Vertex.hpp"

#include <vulkan/vulkan.h>

VkFormat getFormatFromEnum(VertexAttributeEnum attribute)
{
	switch (attribute)
	{
		case POSITION:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case NORMAL:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case COLOR:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case TEXCOORD:
			return VK_FORMAT_R32G32_SFLOAT;
		case TANGENT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case JOINTS:
			return VK_FORMAT_R32G32B32A32_UINT;
		case WEIGHTS:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
	}
	return VK_FORMAT_UNDEFINED;
}

uint32_t attributeEnumToSize(VertexAttributeEnum venum)
{
	switch (venum)
	{
		case VertexAttributeEnum::POSITION:
			return 3;
		case VertexAttributeEnum::NORMAL:
			return 3;
		case VertexAttributeEnum::COLOR:
			return 4;
		case VertexAttributeEnum::TEXCOORD:
			return 2;
		case VertexAttributeEnum::TANGENT:
			return 3;
		case VertexAttributeEnum::JOINTS:
			return 4;
		case VertexAttributeEnum::WEIGHTS:
			return 4;
		default:
			return 0;
	}
}

VertexAttributeInfo getAttributeInfo(VertexAttributeEnum attribute)
{
	switch (attribute)
	{
		case VertexAttributeEnum::POSITION:
			return { VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3) };
		case VertexAttributeEnum::NORMAL:
			return { VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3) };
		case VertexAttributeEnum::COLOR:
			return { VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(glm::vec4) };
		case VertexAttributeEnum::TEXCOORD:
			return { VK_FORMAT_R32G32_SFLOAT, sizeof(glm::vec2) };
		case VertexAttributeEnum::TANGENT:
			return { VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3) };
		case VertexAttributeEnum::JOINTS:
			return { VK_FORMAT_R32G32B32A32_UINT, sizeof(glm::uvec4) };
		case VertexAttributeEnum::WEIGHTS:
			return { VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(glm::vec4) };
		default:
			return { VK_FORMAT_UNDEFINED, 0 };
	}
}

uint32_t calculateOffset(VertexAttributeEnum attribute)
{
	uint32_t offset = 0;

	for (uint16_t e = VertexAttributeEnum::POSITION; e < (uint16_t)attribute; e++)
	{
		offset += attributeEnumToSize((VertexAttributeEnum)e);
	}
	return offset;
}