#pragma once
#include <gtest/gtest.h>
#include <BasicAttributes.hpp>
#include <RehtiEngine.hpp>

TEST(ModelLoaderTests, LoadModels) {
	EXPECT_EQ(RehtiEngine::initializeRehti(), 0);
	ResourceManager& resourceManager = RehtiEngine::getSubSystem<ResourceManager>();
	std::filesystem::path resourcePathBase = ResourceManager::getDefaultResourcePath();
	std::filesystem::path modelPath = resourcePathBase / "models" / "testing" / "CompareNormal.glb";
	if (!std::filesystem::exists(modelPath))
	{
		std::cout << "The model  path " << modelPath << " does not exist." << std::endl;
		std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
		FAIL() << "Model path does not exist.";
	}
	ResourceHandle handle(modelPath);
	std::shared_ptr<GraphicsAsset> asset = resourceManager.get<GraphicsAsset>(handle);
	EXPECT_FALSE(asset->mesh.positions.empty());
	RehtiEngine::cleanupRehti();
}