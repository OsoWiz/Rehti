#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include <EngineSubsystem.hpp>
#include "RehtiAsset.hpp"

class ResourceBuilder
{

	ResourceBuilder& addGraphicsAsset(flecs::entity& entity, const GraphicsAsset& asset);
	ResourceBuilder& addAudioAsset(flecs::entity& entity, const AudioAsset& asset);

	ResourceBuilder();

private:

};

/**
 * @brief Resource manager is used to load various resources.
 */
class ResourceManager : public EngineSubsystem<ResourceManager>
{

public:
	ResourceManager(flecs::world& world);
	~ResourceManager();

	// What do we return? A handle to the loaded resource? Or a report on what was loaded?
	GraphicsAsset loadGltfAsset(const std::filesystem::path& filepath);
	flecs::entity loadGltfAssetAsEntity(const std::filesystem::path& filepath);

	int initialize(const Configuration& config) override;
	int cleanup() override;
	bool isInitialized() const override;
	// void createGraphicsEntity(std::string resourcePath);

private:
	struct Backend;
	friend struct Backend;
	std::unique_ptr<Backend> backendInstance;
};