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
	size += positions.size() * sizeof(VertexAttributes::Position);
	size += normals.size() * sizeof(VertexAttributes::Normal);
	size += colors.size() * sizeof(VertexAttributes::Color);
	size += texCoords.size() * sizeof(VertexAttributes::TexCoord);
	size += tangents.size() * sizeof(VertexAttributes::Tangent);
	size += bitangents.size() * sizeof(VertexAttributes::Bitangent);
	size += joints.size() * sizeof(VertexAttributes::Joints);
	size += weights.size() * sizeof(VertexAttributes::Weights);
	return size;
}

bool Mesh::isValid() const
{
	size_t vertices = positions.size();
	bool valid = vertices > 0;
	VertexAttributeFlags flags = getAvailableVertexAttributes();
	for (VertexAttributeFlags flag = VertexAttributeFlags::POSITION; flag <= VertexAttributeFlags::UNDEFINED; flag = static_cast<VertexAttributeFlags>(static_cast<uint16_t>(flag) << 1))
	{
		 // TODO fix this is useless.
		if ((flag & flags) != VertexAttributeFlags::NONE && vertices != getSize() / sizeof(flag))
		{
			valid = false;
			break;
		}
	}
	return valid;
}
