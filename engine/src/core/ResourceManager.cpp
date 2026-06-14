#include "ResourceManager.hpp"
#include "AssetLoader.hpp"
#include "Logger.hpp"

#include <iostream>

struct ResourceManager::Backend
{
	std::unique_ptr<AssetLoader> graphicsAssetLoader;

	Backend()
	{
		graphicsAssetLoader = std::make_unique<AssetLoader>();
	}
};

ResourceManager::ResourceManager(flecs::world& world)
	: EngineSubsystem(world), backendInstance(std::make_unique<Backend>())
{
}

ResourceManager::~ResourceManager()
{
}

GraphicsAsset ResourceManager::loadGltfAsset(const std::filesystem::path& filepath)
{
	GraphicsAsset asset{};
	std::vector<GraphicsAssetInternal> assets = this->backendInstance->graphicsAssetLoader->loadModel(filepath.string());
	Logger::instance() << "Loaded " << assets.size() << " assets from " << filepath.string() << std::endl;
	for (const auto& a : assets)
	{
		Logger::instance() << "Asset has " << a.mesh.positions.size() << " vertices and " << a.mesh.indices.size() << " indices." << std::endl;
		asset.mesh.positions.insert(asset.mesh.positions.end(), a.mesh.positions.begin(), a.mesh.positions.end());
	}
	return asset;
}


flecs::entity ResourceManager::loadGltfAssetAsEntity(const std::filesystem::path& filepath)
{
	auto asset = loadGltfAsset(filepath);
	flecs::entity entity = flecs::entity(world);
	entity.set<GraphicsAsset>(asset);
	return entity;
}

int ResourceManager::initialize(const Configuration& config)
{
	return 0;
}

int ResourceManager::cleanup()
{
	return 0;
}

bool ResourceManager::isInitialized() const
{
	return false;
}
