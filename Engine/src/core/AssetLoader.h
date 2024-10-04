#pragma once
#include "Vertex.h"
#include "GraphicsTypes.h"

#include <string>
#include <vector>

struct GraphicsAsset
{
	const std::vector <Vertex<Position, Normal, Color, TexCoord, Tangent, Joints, Weights>> vertices;
	const std::vector <uint32_t> indices;
	std::set<VertexAttributeEnum> attributes;
	std::vector<Animation> animations;
};

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

	std::vector<GraphicsAsset> loadModel(std::string path);

private:

};

