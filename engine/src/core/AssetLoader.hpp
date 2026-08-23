#pragma once
#include "RehtiAsset.hpp"
#include <optional>
#include <filesystem>
#include <string>
#include <memory>

namespace AssetLoader
{
	// Legacy load for old graphics asset loader. Todo change naming at least.
	std::vector<GraphicsAssetInternal> load(std::filesystem::path path);

	// Free function template for generic resource loading
	// Specializations should be implemented in respective loader .cpp files
	template<typename ResourceType>
	std::shared_ptr<ResourceType> load(const std::filesystem::path& path);

	// Specialization for GraphicsAsset
	template<>
	std::shared_ptr<GraphicsAsset> load<GraphicsAsset>(const std::filesystem::path& path);

}

