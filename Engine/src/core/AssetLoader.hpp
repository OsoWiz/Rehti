#include "GraphicsTypesInternal.hpp"
#include "RehtiAsset.hpp"
#include <Vertex.hpp>
#include <optional>
#include <string>

class AssetLoader
{
public:
	AssetLoader();
	~AssetLoader();

	std::vector<GraphicsAssetInternal> loadModel(std::string path);

private:

};

