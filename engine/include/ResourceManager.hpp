#pragma once
#include <string>
#include <memory>
#include <filesystem>
#include <variant>
#include <EngineSubsystem.hpp>
#include "AssetLoader.hpp"
#include "RehtiException.hpp"


struct ResourceHandle
{
	std::variant<uint64_t, std::filesystem::path> identifier;
};

/**
 * @brief Resource manager is for handling various resources games use.
 * Uses free function templates AssetLoader::load<T>(path) for actual resource loading. So you can extend loading by defining such function.
 * Caching is delegated to the caller via flecs entities. Todo make loading registrable (runtime)
 */
class ResourceManager : public EngineSubsystem<ResourceManager>
{
public:

	// TODO perhaps revisit the exception strategy. Tryload sounds like there should be a try.
	template<typename ResourceType>
	static inline std::shared_ptr<ResourceType> tryLoad(const std::filesystem::path& path)
	{
		if (std::filesystem::exists(path))
		{
			return AssetLoader::load<ResourceType>(path);
		}
		else
		{
			return nullptr;
		}
	}


	ResourceManager(flecs::world& world);
	~ResourceManager();

	/**
	 * @brief Returns the rehti engine resources path.
	 * @return path to "engine/resources" directory on the installed machine.
	 */
	static std::filesystem::path getDefaultResourcePath();


	template<typename ResourceType>
	std::shared_ptr<ResourceType> get(const ResourceHandle& handle);

	// Legacy graphics asset loading methods
	GraphicsAsset loadGltfAsset(const std::filesystem::path& filepath);
	flecs::entity loadGltfAssetAsEntity(const std::filesystem::path& filepath);

	int initialize(const Configuration& config) override;
	int preDestroy() override;
	bool isInitialized() const override;

private:
	struct Backend;
	friend struct Backend;
	std::unique_ptr<Backend> backendInstance;
};

// Template method implementation. Get is allowed to throw since you know you are trying to GET something.
template<typename ResourceType>
std::shared_ptr<ResourceType> ResourceManager::get(const ResourceHandle& handle)
{
	// If handle contains an entity ID, get it from flecs world
	if (std::holds_alternative<uint64_t>(handle.identifier))
	{
		uint64_t entityId = std::get<uint64_t>(handle.identifier);
		flecs::entity entity = world.entity(entityId);
		if (!entity.is_alive())
		{
			throw RehtiException(RehtiError::RESOURCE_NOT_FOUND, "Entity does not exist in flecs world");
		}
		std::shared_ptr<ResourceType> resourcePtr = entity.get<std::shared_ptr<ResourceType>>();
		if (!resourcePtr)
		{
			throw RehtiException(RehtiError::RESOURCE_NOT_FOUND, "Entity does not have the requested resource type");
		}
		return resourcePtr;
	}

	// If handle contains a filesystem path, load it
	std::filesystem::path resourcePath = std::get<std::filesystem::path>(handle.identifier);
	std::filesystem::path normalizedPath = std::filesystem::absolute(resourcePath);

	// Call the free function template specialization
	return tryLoad<ResourceType>(normalizedPath);
}