#include <RehtiAsset.hpp>

VertexAttributeFlags Mesh::getAvailableVertexAttributes() const
{
	VertexAttributeFlags flags = VertexAttributeFlags::NONE;
	if (!positions.empty()) flags |= VertexAttributeFlags::POSITION;
	if (!normals.empty()) flags |= VertexAttributeFlags::NORMAL;
	if (!colors.empty()) flags |= VertexAttributeFlags::COLOR;
	if (!texCoords.empty()) flags |= VertexAttributeFlags::TEXCOORD;
	if (!tangents.empty()) flags |= VertexAttributeFlags::TANGENT;
	if (!bitangents.empty()) flags |= VertexAttributeFlags::BITANGENT;
	if (!joints.empty()) flags |= VertexAttributeFlags::JOINTS;
	if (!weights.empty()) flags |= VertexAttributeFlags::WEIGHTS;

    return flags; // the returned type is not a member of the enumclass incase multiple attributes are present. Is this a problem?
}

size_t Mesh::getSize() const
{
	size_t size = 0;
	size += positions.size() * VertexAttributes::Position::getSize();
	size += normals.size() * VertexAttributes::Normal::getSize();
	size += colors.size() * VertexAttributes::Color::getSize();
	size += texCoords.size() * VertexAttributes::TexCoord::getSize();
	size += tangents.size() * VertexAttributes::Tangent::getSize();
	size += bitangents.size() * VertexAttributes::Bitangent::getSize();
	size += joints.size() * VertexAttributes::Joints::getSize();
	size += weights.size() * VertexAttributes::Weights::getSize();
	return size;
}

size_t Mesh::getIndexSize() const
{
	return indices.size() * sizeof(uint32_t);
}

size_t Mesh::getStride() const
{
	size_t size = 0;
	size += static_cast<size_t>(positions.empty() ? 0 : 1) * VertexAttributes::Position::getSize();
	size += static_cast<size_t>(normals.empty() ? 0 : 1) * VertexAttributes::Normal::getSize();
	size += static_cast<size_t>(colors.empty() ? 0 : 1) * VertexAttributes::Color::getSize();
	size += static_cast<size_t>(texCoords.empty() ? 0 : 1) * VertexAttributes::TexCoord::getSize();
	size += static_cast<size_t>(tangents.empty() ? 0 : 1) * VertexAttributes::Tangent::getSize();
	size += static_cast<size_t>(bitangents.empty() ? 0 : 1) * VertexAttributes::Bitangent::getSize();
	size += static_cast<size_t>(joints.empty() ? 0 : 1) * VertexAttributes::Joints::getSize();
	size += static_cast<size_t>(weights.empty() ? 0 : 1) * VertexAttributes::Weights::getSize();
	return size;
}

bool Mesh::isValid() const
{
	size_t vertices = positions.size();
	return vertices > 0;
}

bool Mesh::calculateTangents()
{
	if (positions.empty() || normals.empty() || texCoords.empty()
		|| (!tangents.empty() && !bitangents.empty()))
	{
		return false; // Cannot calculate tangents without positions, normals, and texture coordinates
	}
	tangents.resize(positions.size());
	bitangents.resize(positions.size());

	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		uint32_t index0 = indices[i];
		uint32_t index1 = indices[i + 1];
		uint32_t index2 = indices[i + 2];
		const glm::vec3& pos0 = positions[index0].value;
		const glm::vec3& pos1 = positions[index1].value;
		const glm::vec3& pos2 = positions[index2].value;
		const glm::vec2& uv0 = texCoords[index0].value;
		const glm::vec2& uv1 = texCoords[index1].value;
		const glm::vec2& uv2 = texCoords[index2].value;
		glm::vec3 edge1 = pos1 - pos0;
		glm::vec3 edge2 = pos2 - pos0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		glm::vec3 tangent, bitangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);
		tangents[index0].value = tangent;
		tangents[index1].value = tangent;
		tangents[index2].value = tangent;
		bitangents[index0].value = bitangent;
		bitangents[index1].value = bitangent;
		bitangents[index2].value = bitangent;
	}
	return true;
}

VertexAttributeFlags ShaderInterface::getLikelyVertexAttributes() const
{
	VertexAttributeFlags flags = VertexAttributeFlags::NONE;
	for (const auto& input : inputs)
	{
		switch (input.format)
		{
			case Rehti::Format::Vec3:
				if (input.location == 0) // Assuming location 0 is for position
					flags |= VertexAttributeFlags::POSITION; // Assuming this format is used for position
				else if (input.location == 1) // Assuming location 1 is for normal
					flags |= VertexAttributeFlags::NORMAL; // Assuming this format is used for normal
				else if (input.location == 4) // Assuming location 4 is for tangent
					flags |= VertexAttributeFlags::TANGENT; // Assuming this format is used for tangent
				else if (input.location == 5) // Assuming location 5 is for bitangent
					flags |= VertexAttributeFlags::BITANGENT; // Assuming this format is used for bitangent
				break;
			case Rehti::Format::Vec4:
				if (input.location == 3) // Assuming location 3 is for color
					flags |= VertexAttributeFlags::COLOR; 
				if (input.location == 7)
					flags |= VertexAttributeFlags::WEIGHTS;
				break;
			case Rehti::Format::Vec2:
				if (input.location == 2) 
					flags |= VertexAttributeFlags::TEXCOORD; 
				break;
			case Rehti::Format::UVec4:
				if (input.location == 6)
					flags |= VertexAttributeFlags::JOINTS;
				break;
			default:
				break;
		}
	}
	return flags;
}
