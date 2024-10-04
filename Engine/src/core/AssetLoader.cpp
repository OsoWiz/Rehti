#include "AssetLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <map>
#include <queue>

// Helper functionality

inline glm::vec2 aiVecToGlmVec(aiVector2D vec)
{
	return glm::vec2(vec.x, vec.y);
}

inline glm::vec3 aiVecToGlmVec(aiVector3D vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::vec4 aiVecToGlmVec(aiColor4D vec)
{
	return glm::vec4(vec.r, vec.g, vec.b, vec.a);
}

inline glm::mat4 aiMatToGlmMat(aiMatrix4x4 mat)
{
	return glm::transpose(glm::mat4(mat.a1, mat.a2, mat.a3, mat.a4,
		mat.b1, mat.b2, mat.b3, mat.b4,
		mat.c1, mat.c2, mat.c3, mat.c4,
		mat.d1, mat.d2, mat.d3, mat.d4));
}

AssetLoader::AssetLoader()
{
}

AssetLoader::~AssetLoader()
{
}

size_t loadAnimations(const aiScene* scene, std::vector<Animation>& animationsToFill)
{
	size_t loadedAnimations = 0;

	for (uint32_t i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];



	}

	return loadedAnimations;
}

size_t fillSkeleton(aiNode* root, std::vector<BoneNode>& bonesToFill, std::vector<glm::mat4> transformations, std::map<std::string, uint32_t>& nameToIndex)
{
	// bonecount acts as the next added index as well as count of bones
	size_t boneCount = 0;
	uint32_t level = 0;
	// first bone has no parent
	BoneNode bone{};
	bone.parent = -1;
	bonesToFill.push_back(bone);
	// add name to index map
	std::string rootName = std::string(root->mName.C_Str());
	nameToIndex[rootName] = 0;
	boneCount++;
	std::queue<aiNode*> nodeQueue;
	nodeQueue.push(root);
	// breadth first add
	while (!nodeQueue.empty())
	{
		// deque
		aiNode* currentNode = nodeQueue.front();
		nodeQueue.pop();
		// push current transformation of the bone to the matrix list
		transformations.push_back(aiMatToGlmMat(currentNode->mTransformation));

		// add children to queue if any
		uint32_t numChildren = currentNode->mNumChildren;
		int parentIndex = nameToIndex[std::string(currentNode->mName.C_Str())];
		if (0 < numChildren)
		{
			for (uint32_t i = 0; i < numChildren; i++)
			{
				// create a bone node
				BoneNode newNode{};
				newNode.parent = parentIndex;
				std::string name = std::string(currentNode->mChildren[i]->mName.C_Str());
				if (name.find("Ctrl") != std::string::npos) // do not add control nodes
				{
					continue;
				}
				// set children of parent
				bonesToFill[parentIndex].children.push_back(boneCount);
				// map name to index
				nameToIndex[name] = boneCount;
				// add the bone of the child to the list
				bonesToFill.push_back(newNode);
				// increment bonecount and check children for more bones
				boneCount++;
				nodeQueue.push(currentNode->mChildren[i]);
			}
		}
	}
	return boneCount;
}

std::vector<GraphicsAsset> AssetLoader::loadModel(std::string path)
{
	std::vector<GraphicsAsset> assets;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		std::vector<Vertex<Position, Normal, Color, TexCoord, Tangent, Bitangent, Joints, Weights>> vertices(mesh->mNumVertices);
		std::vector<uint32_t> indices;
		std::set<VertexAttributeEnum> attributes;
		// check features
		if (mesh->HasPositions())
			attributes.insert(VertexAttributeEnum::POSITION);
		if (mesh->HasNormals())
			attributes.insert(VertexAttributeEnum::NORMAL);
		if (mesh->GetNumColorChannels() > 0)
			attributes.insert(VertexAttributeEnum::COLOR);
		if (mesh->HasTextureCoords(0))
			attributes.insert(VertexAttributeEnum::TEXCOORD);
		if (mesh->HasTangentsAndBitangents())
		{
			attributes.insert(VertexAttributeEnum::TANGENT);
			attributes.insert(VertexAttributeEnum::BITANGENT);
		}
		if (mesh->HasBones())
		{
			attributes.insert(VertexAttributeEnum::JOINTS);
			attributes.insert(VertexAttributeEnum::WEIGHTS);
			// handle conversion of bone data to format that can be looped over
			// getJoints(mesh)
			// getWeights(mesh)
		}

		for (uint32_t vi = 0u; i < mesh->mNumVertices; i++)
		{
			Vertex<Position, Normal, Color, TexCoord, Tangent, Bitangent, Joints, Weights> vertex;
			vertex.get<Position>() = aiVecToGlmVec(mesh->mVertices[vi]);

			if (mesh->HasNormals())
				vertex.get<Normal>() = aiVecToGlmVec(mesh->mNormals[vi]);
			if (mesh->HasVertexColors(0))
				vertex.get<Color>() = aiVecToGlmVec(mesh->mColors[0][vi]);
			if (mesh->HasTextureCoords(0))
				vertex.get<TexCoord>() = aiVecToGlmVec(mesh->mTextureCoords[0][vi]);
			if (mesh->HasTangentsAndBitangents())
			{
				vertex.get<Tangent>() = aiVecToGlmVec(mesh->mTangents[vi]);
				vertex.get<Bitangent>() = aiVecToGlmVec(mesh->mBitangents[vi]);
			}
		} // end of vertices for;


	}

	return assets;
}
