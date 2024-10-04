#include "Vertex.h"


VkFormat RehtiGraphics::getFormatFromEnum(RehtiGraphics::VertexAttributeEnum attribute)
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

uint32_t attributeEnumToSize(RehtiGraphics::VertexAttributeEnum venum)
{ // todo just ask the size of the vec somehow?
	switch (venum)
	{
		case RehtiGraphics::VertexAttributeEnum::POSITION:
			return 3;
		case RehtiGraphics::VertexAttributeEnum::NORMAL:
			return 3;
		case RehtiGraphics::VertexAttributeEnum::COLOR:
			return 4;
		case RehtiGraphics::VertexAttributeEnum::TEXCOORD:
			return 2;
		case RehtiGraphics::VertexAttributeEnum::TANGENT:
			return 3;
		case RehtiGraphics::VertexAttributeEnum::JOINTS:
			return 4;
		case RehtiGraphics::VertexAttributeEnum::WEIGHTS:
			return 4;
		default:
			return 0;
	}
}

uint32_t calculateOffset(RehtiGraphics::VertexAttributeEnum attribute)
{
	uint32_t offset = 0;

	for (uint16_t e = RehtiGraphics::VertexAttributeEnum::POSITION; e < (uint16_t)attribute; e++)
	{
		offset += attributeEnumToSize((RehtiGraphics::VertexAttributeEnum)e);
	}
	return offset;
}

std::vector<VkVertexInputAttributeDescription> RehtiGraphics::getVertexAttributes(std::set<VertexAttributeEnum> attributes, uint32_t vertexBinding)
{
	std::vector<VkVertexInputAttributeDescription> selectedAttributes;
	for (auto attribute : attributes)
	{
		VkFormat format = RehtiGraphics::getFormatFromEnum(attribute);
		if (format != VK_FORMAT_UNDEFINED)
		{
			VkVertexInputAttributeDescription newAttribute{};
			newAttribute.binding = vertexBinding;
			newAttribute.location = selectedAttributes.size();
			newAttribute.format = format;
			newAttribute.offset = calculateOffset(attribute); // still pretty ass? 
			selectedAttributes.push_back(newAttribute);
		}
	}

	return selectedAttributes;
}