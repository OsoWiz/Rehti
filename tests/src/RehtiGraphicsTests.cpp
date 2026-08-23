#include <gtest/gtest.h>
#include <RehtiGraphics.hpp>
#include <RehtiEngine.hpp>
#include <RehtiAsset.hpp>
#include <GraphicsTypes.hpp>
#include <glm/glm.hpp>

class RehtiGraphicsTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		// Initialize the engine before each test
		EXPECT_EQ(RehtiEngine::initializeRehti(), 0);
	}

	void TearDown() override
	{
		// Clean up after each test
		RehtiEngine::cleanupRehti();
	}
};

// Test graphics object creation with a simple mesh
TEST_F(RehtiGraphicsTest, CreateGraphicsObject)
{
	auto& graphics = RehtiEngine::getSubSystem<RehtiGraphics>();
	EXPECT_TRUE(graphics.isInitialized());

	// Create a simple mesh
	Mesh testMesh;
	testMesh.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 0.0f, 0.0f) });
	testMesh.positions.push_back(VertexAttributes::Position{ glm::vec3(1.0f, 0.0f, 0.0f) });
	testMesh.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 1.0f, 0.0f) });
	testMesh.indices = { 0, 1, 2 };

	// Test graphics object creation
	GraphicsObjectHandle handle = graphics.createGraphicsObject(testMesh);
	EXPECT_NE(handle.id, 0);
}

// Test graphics object creation with multiple meshes
TEST_F(RehtiGraphicsTest, CreateMultipleGraphicsObjects)
{
	auto& graphics = RehtiEngine::getSubSystem<RehtiGraphics>();
	EXPECT_TRUE(graphics.isInitialized());

	// Create first mesh
	Mesh mesh1;
	mesh1.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 0.0f, 0.0f) });
	mesh1.positions.push_back(VertexAttributes::Position{ glm::vec3(1.0f, 0.0f, 0.0f) });
	mesh1.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 1.0f, 0.0f) });
	mesh1.indices = { 0, 1, 2 };

	// Create second mesh
	Mesh mesh2;
	mesh2.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 0.0f, 0.0f) });
	mesh2.positions.push_back(VertexAttributes::Position{ glm::vec3(2.0f, 0.0f, 0.0f) });
	mesh2.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 2.0f, 0.0f) });
	mesh2.indices = { 0, 1, 2 };

	// Create graphics objects
	GraphicsObjectHandle handle1 = graphics.createGraphicsObject(mesh1);
	GraphicsObjectHandle handle2 = graphics.createGraphicsObject(mesh2);

	// Verify both handles are valid and different
	EXPECT_NE(handle1.id, 0);
	EXPECT_NE(handle2.id, 0);
	EXPECT_NE(handle1.id, handle2.id);
}

// Test graphics pipeline creation with valid configuration
TEST_F(RehtiGraphicsTest, CreateGraphicsPipeline)
{
	auto& graphics = RehtiEngine::getSubSystem<RehtiGraphics>();
	EXPECT_TRUE(graphics.isInitialized());

	// Create a minimal graphics pipeline configuration
	GraphicsPipelineConfig pipelineConfig;
	auto& resourceMngr = RehtiEngine::getSubSystem<ResourceManager>();
	ShaderAsset vertexShaderAsset{};
	ShaderAsset fragmentShaderAsset{};
	// Configure vertex shader (required)
	pipelineConfig.vertexShader.shaderAsset = vertexShaderAsset;

	// Configure fragment shader (required)
	pipelineConfig.fragmentShader.shaderAsset = fragmentShaderAsset;

	// Use default rasterization and depth stencil configs
	pipelineConfig.rasterizationConfig.polygonFillMode = RasterizationConfig::PolygonFillMode::FILL;
	pipelineConfig.rasterizationConfig.cullMode = RasterizationConfig::CullMode::BACK;

	// Test pipeline creation
	// PipelineHandle handle = graphics.createGraphicsPipeline(pipelineConfig);
	// EXPECT_NE(handle.id, 0);
	EXPECT_TRUE(true); // TODO fix after finishing resource manager and loading interfaces.
}

// Test graphics pipeline creation with tessellation shaders
TEST_F(RehtiGraphicsTest, CreateGraphicsPipelineWithTessellation)
{
	auto& graphics = RehtiEngine::getSubSystem<RehtiGraphics>();
	EXPECT_TRUE(graphics.isInitialized());

	// Create pipeline configuration with tessellation
	GraphicsPipelineConfig pipelineConfig;

	// Configure vertex shader
	/*pipelineConfig.vertexShader.shaderAsset.filePath = "shaders/tessellation.vert";
	pipelineConfig.vertexShader.shaderAsset.shaderType = ShaderType::VERTEX;*/

	// Configure tessellation control shader
	//ShaderAsset tessControlShader;
	//tessControlShader.filePath = "shaders/tessellation.tesc";
	//tessControlShader.shaderType = ShaderType::TESSELLATION_CONTROL;
	//pipelineConfig.tessellationControlShader = PipelineShader{ tessControlShader, ShaderInterface{} };

	//// Configure tessellation evaluation shader
	//ShaderAsset tessEvalShader;
	//tessEvalShader.filePath = "shaders/tessellation.tese";
	//tessEvalShader.shaderType = ShaderType::TESSELLATION_EVALUATION;
	//pipelineConfig.tessellationEvaluationShader = PipelineShader{ tessEvalShader, ShaderInterface{} };

	//// Configure fragment shader
	//pipelineConfig.fragmentShader.shaderAsset.filePath = "shaders/tessellation.frag";
	//pipelineConfig.fragmentShader.shaderAsset.shaderType = ShaderType::FRAGMENT;

	// Test pipeline creation with tessellation
	// PipelineHandle handle = graphics.createGraphicsPipeline(pipelineConfig);
	// EXPECT_NE(handle.id, 0);
	EXPECT_TRUE(true); // TODO fix after finishing resource manager and loading interfaces.
}

// Test multiple pipeline creation
TEST_F(RehtiGraphicsTest, CreateMultipleGraphicsPipelines)
{
	auto& graphics = RehtiEngine::getSubSystem<RehtiGraphics>();
	EXPECT_TRUE(graphics.isInitialized());

	// Create first pipeline
	GraphicsPipelineConfig config1;
	//config1.vertexShader.shaderAsset.filePath = "shaders/shader1.vert";
	//config1.vertexShader.shaderAsset.shaderType = ShaderType::VERTEX;
	//config1.fragmentShader.shaderAsset.filePath = "shaders/shader1.frag";
	//config1.fragmentShader.shaderAsset.shaderType = ShaderType::FRAGMENT;
	//config1.rasterizationConfig.polygonFillMode = RasterizationConfig::PolygonFillMode::FILL;

	//// Create second pipeline with different configuration
	//GraphicsPipelineConfig config2;
	//config2.vertexShader.shaderAsset.filePath = "shaders/shader2.vert";
	//config2.vertexShader.shaderAsset.shaderType = ShaderType::VERTEX;
	//config2.fragmentShader.shaderAsset.filePath = "shaders/shader2.frag";
	//config2.fragmentShader.shaderAsset.shaderType = ShaderType::FRAGMENT;
	//config2.rasterizationConfig.polygonFillMode = RasterizationConfig::PolygonFillMode::LINE;

	// Create both pipelines
	//PipelineHandle handle1 = graphics.createGraphicsPipeline(config1);
	//PipelineHandle handle2 = graphics.createGraphicsPipeline(config2);

	//// Verify both handles are valid and different
	//EXPECT_NE(handle1.id, 0);
	//EXPECT_NE(handle2.id, 0);
	//EXPECT_NE(handle1.id, handle2.id);
}

// Test attaching graphics object to pipeline
TEST_F(RehtiGraphicsTest, AttachGraphicsObjectToPipeline)
{
	auto& graphics = RehtiEngine::getSubSystem<RehtiGraphics>();
	EXPECT_TRUE(graphics.isInitialized());

	// Create a graphics object
	Mesh testMesh;
	testMesh.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 0.0f, 0.0f) });
	testMesh.positions.push_back(VertexAttributes::Position{ glm::vec3(1.0f, 0.0f, 0.0f) });
	testMesh.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 1.0f, 0.0f) });
	testMesh.indices = { 0, 1, 2 };
	GraphicsObjectHandle objHandle = graphics.createGraphicsObject(testMesh);

	// Create a graphics pipeline
	GraphicsPipelineConfig pipelineConfig;
	//pipelineConfig.vertexShader.shaderAsset.filePath = "shaders/default.vert";
	//pipelineConfig.vertexShader.shaderAsset.shaderType = ShaderType::VERTEX;
	//pipelineConfig.fragmentShader.shaderAsset.filePath = "shaders/default.frag";
	//pipelineConfig.fragmentShader.shaderAsset.shaderType = ShaderType::FRAGMENT;
	//PipelineHandle pipelineHandle = graphics.createGraphicsPipeline(pipelineConfig);

	//// Test attachment
	//bool attachmentResult = graphics.attachGraphicsObjectToPipeline(pipelineHandle, objHandle);
	//EXPECT_TRUE(attachmentResult);
	EXPECT_TRUE(true); // TODO fix after finishing resource manager and loading interfaces.
}

// Test multiple object attachments to same pipeline
TEST_F(RehtiGraphicsTest, AttachMultipleObjectsToPipeline)
{
	auto& graphics = RehtiEngine::getSubSystem<RehtiGraphics>();
	EXPECT_TRUE(graphics.isInitialized());

	// Create multiple graphics objects
	//Mesh mesh1;
	//mesh1.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 0.0f, 0.0f) });
	//mesh1.positions.push_back(VertexAttributes::Position{ glm::vec3(1.0f, 0.0f, 0.0f) });
	//mesh1.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 1.0f, 0.0f) });
	//mesh1.indices = { 0, 1, 2 };

	//Mesh mesh2;
	//mesh2.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 0.0f, 0.0f) });
	//mesh2.positions.push_back(VertexAttributes::Position{ glm::vec3(2.0f, 0.0f, 0.0f) });
	//mesh2.positions.push_back(VertexAttributes::Position{ glm::vec3(0.0f, 2.0f, 0.0f) });
	//mesh2.indices = { 0, 1, 2 };

	//GraphicsObjectHandle objHandle1 = graphics.createGraphicsObject(mesh1);
	//GraphicsObjectHandle objHandle2 = graphics.createGraphicsObject(mesh2);

	//// Create a graphics pipeline
	//GraphicsPipelineConfig pipelineConfig;
	//pipelineConfig.vertexShader.shaderAsset.filePath = "shaders/default.vert";
	//pipelineConfig.vertexShader.shaderAsset.shaderType = ShaderType::VERTEX;
	//pipelineConfig.fragmentShader.shaderAsset.filePath = "shaders/default.frag";
	//pipelineConfig.fragmentShader.shaderAsset.shaderType = ShaderType::FRAGMENT;
	//PipelineHandle pipelineHandle = graphics.createGraphicsPipeline(pipelineConfig);

	//// Attach both objects to the same pipeline
	//bool attachment1 = graphics.attachGraphicsObjectToPipeline(pipelineHandle, objHandle1);
	//bool attachment2 = graphics.attachGraphicsObjectToPipeline(pipelineHandle, objHandle2);

	//EXPECT_TRUE(attachment1);
	//EXPECT_TRUE(attachment2);
	// TODO fix after finishing resource manager and loading interfaces.
}
