#pragma once
#include <gtest/gtest.h>
#include <BasicAttributes.hpp>
#include <RehtiEngine.hpp>

TEST(ModelLoaderTests, LoadModels) {
	EXPECT_EQ(RehtiEngine::initializeRehti(), 0);
	ResourceManager& resourceManager = RehtiEngine::getSubSystem<ResourceManager>();
	std::filesystem::path modelPath = "Rehti/engine/resources/models/testing";
	GraphicsAsset asset = resourceManager.loadGltfAsset(modelPath);
	EXPECT_FALSE(asset.mesh.positions.empty());
	RehtiEngine::cleanupRehti();
}