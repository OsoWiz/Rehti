#include "GraphicsTypes.hpp"
#include <Vertex.hpp>
#include <optional>
#include <string>

struct GraphicsAsset
{
	std::vector <FullVertex> vertices;
	std::vector <uint32_t> indices;
	VertexAttributeFlags attributes;
	std::vector<Animation> animations;
	std::optional<Skeleton> skeleton;
};

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

	std::vector<GraphicsAsset> loadModel(std::string path);

private:

};

