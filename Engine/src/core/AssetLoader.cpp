#include "AssetLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <BasicAttributes.hpp>

#include <iostream>
#include <map>
#include <queue>

// Helper functionality

template <typename T>
inline bool floatsEqual(T a, T b)
{
	return std::abs(a - b) < std::numeric_limits<T>::epsilon();
}


inline glm::vec2 aiToGlm(aiVector2D vec)
{
	return glm::vec2(vec.x, vec.y);
}

inline glm::vec3 aiToGlm(aiVector3D vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::vec4 aiToGlm(aiColor4D vec)
{
	return glm::vec4(vec.r, vec.g, vec.b, vec.a);
}

inline glm::quat aiToGlm(aiQuaternion quat)
{
	return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

inline glm::mat4 aiToGlm(aiMatrix4x4 mat)
{
	return glm::transpose(glm::mat4(mat.a1, mat.a2, mat.a3, mat.a4,
		mat.b1, mat.b2, mat.b3, mat.b4,
		mat.c1, mat.c2, mat.c3, mat.c4,
		mat.d1, mat.d2, mat.d3, mat.d4));
}

inline float interpolatedTimeFactor(const double now, const double prevTimeFrame, const double frameDiff, const double totalTime)
{
	double diff = now - prevTimeFrame;
	if (diff < 0.0)
	{
		diff += totalTime;
	}
	return diff / frameDiff;
}

inline glm::vec3 interpolateLinear(const glm::vec3& start, const glm::vec3& end, float timeFactor)
{
	return (1.f - timeFactor) * start + timeFactor * end;
}

inline glm::quat interpolateLinear(const glm::quat& start, const glm::quat& end, float timeFactor)
{
	return glm::slerp(start, end, timeFactor);
}

inline glm::vec3 interpolateAIVectorkeys(const aiVectorKey& current, const aiVectorKey& prev, const double now, const double totalTicks)
{
	double frameDiff = current.mTime - prev.mTime;
	if (frameDiff < 0.0)
	{
		frameDiff += totalTicks;
	}
	float factor = (floatsEqual(frameDiff, 0.0)) ? 1.f : interpolatedTimeFactor(now, prev.mTime, frameDiff, totalTicks);
	return interpolateLinear(aiToGlm(prev.mValue), aiToGlm(current.mValue), factor);
}

inline glm::quat interpolateAIQuatkeys(const aiQuatKey& current, const aiQuatKey& prev, const double now, double totalTicks)
{
	double frameDiff = current.mTime - prev.mTime;
	if (frameDiff < 0.0)
	{
		frameDiff += totalTicks;
	}
	float factor = (floatsEqual(frameDiff, 0.0)) ? 1.f : interpolatedTimeFactor(now, prev.mTime, frameDiff, totalTicks);
	return interpolateLinear(aiToGlm(prev.mValue), aiToGlm(current.mValue), factor);
}

AssetLoader::AssetLoader()
{
}

AssetLoader::~AssetLoader()
{
}
/**
 * @brief Function for loading
 * @param scene
 * @param nameToIndex
 * @param animationsToFill
 * @return
 */
size_t loadAnimations(const aiScene* scene, std::map<std::string, uint32_t>& nameToIndex, std::vector<Animation>& animationsToFill)
{
	size_t loadedAnimations = 0;

	for (uint32_t i = 0; i < scene->mNumAnimations; i++)
	{
		Animation newAnimation{};
		aiAnimation* animation = scene->mAnimations[i];
		newAnimation.ticksPerSecond = static_cast<float>(animation->mTicksPerSecond);
		if (animation->mTicksPerSecond > 0)
			newAnimation.duration = static_cast<float>(animation->mDuration / animation->mTicksPerSecond);
		else
			newAnimation.duration = static_cast<float>(animation->mDuration);

		uint32_t numBoneChannels = animation->mNumChannels;
		uint32_t maxKeys = std::max({ animation->mChannels[0]->mNumPositionKeys,
									 animation->mChannels[0]->mNumRotationKeys,
									 animation->mChannels[0]->mNumScalingKeys });
		newAnimation.animationNodes.resize(maxKeys);
		newAnimation.totalTicks = animation->mDuration;
		newAnimation.ticksPerSecond = (0 < animation->mTicksPerSecond) ? animation->mTicksPerSecond : 24;
		newAnimation.duration = animation->mDuration / animation->mTicksPerSecond;
		// for each bone
		for (uint32_t j = 0; j < numBoneChannels; j++)
		{
			aiNodeAnim* animationNode = animation->mChannels[j];
			std::string boneName = std::string(animationNode->mNodeName.C_Str());
			if (!nameToIndex.contains(boneName)) // discard control nodes, etc.
			{
				continue;
			}
			uint32_t index = nameToIndex[boneName];
			uint32_t nSca = animationNode->mNumScalingKeys;
			uint32_t nRot = animationNode->mNumRotationKeys;
			uint32_t nPos = animationNode->mNumPositionKeys;
			uint32_t numKeys = std::max({ nSca,
										 nRot,
										 nPos });
			uint32_t keySum = animationNode->mNumPositionKeys + animationNode->mNumRotationKeys + animationNode->mNumScalingKeys;
			if (maxKeys < numKeys)
			{
				std::cout << "Warning: " << boneName << " has more keys than the previous maximum. " << numKeys << " > " << maxKeys << std::endl;
				maxKeys = numKeys;
				newAnimation.animationNodes.resize(maxKeys);
			}
			uint32_t si = 0;
			uint32_t ri = 0;
			uint32_t ti = 0;
			bool hasScale = 1 < nSca;
			bool hasRotation = 1 < nRot;
			bool hasPosition = 1 < nPos;
			double totalKeys = std::max({ animationNode->mScalingKeys[nSca - 1].mTime,
										 animationNode->mRotationKeys[nRot - 1].mTime,
										 animationNode->mPositionKeys[nPos - 1].mTime });
			uint32_t keyIndex = 0;
			// Until all keys are processed for this bone
			while (keyIndex < numKeys)
			{
				uint32_t psi = (nSca + si - 1) % nSca; // previous scale index
				uint32_t pri = (nRot + ri - 1) % nRot; // previous rotation index
				uint32_t pti = (nPos + ti - 1) % nPos; // previous position index
				aiVectorKey sk = animationNode->mScalingKeys[si];
				aiQuatKey rk = animationNode->mRotationKeys[ri];
				aiVectorKey pk = animationNode->mPositionKeys[ti];
				double st = (hasScale) ? sk.mTime : newAnimation.totalTicks;
				double rt = (hasRotation) ? rk.mTime : newAnimation.totalTicks;
				double pt = (hasPosition) ? pk.mTime : newAnimation.totalTicks;
				double now = std::min({ st, rt, pt });
				// prev
				aiVectorKey psk = animationNode->mScalingKeys[psi];
				aiQuatKey prk = animationNode->mRotationKeys[pri];
				aiVectorKey ppk = animationNode->mPositionKeys[pti];

				Pose pose{};
				pose.scale = interpolateAIVectorkeys(sk, psk, now, totalKeys);
				pose.orientation = interpolateAIQuatkeys(rk, prk, now, totalKeys);
				pose.position = interpolateAIVectorkeys(pk, ppk, now, totalKeys);

				// set the orientation of the bone at keyframe k
				newAnimation.animationNodes[keyIndex].bones[index] = pose;
				newAnimation.animationNodes[keyIndex].time = now;
				// advance the indices
				if (hasScale && floatsEqual(now, sk.mTime))
				{
					si++;
				}
				if (hasRotation && floatsEqual(now, rk.mTime))
				{
					ri++;
				}
				if (hasPosition && floatsEqual(now, pk.mTime))
				{
					ti++;
				}

				keyIndex++;
			} // for each animation key
		}     // end of bone for
	}

	return loadedAnimations;
}

/**
 * @brief Fills a list of bones, transformations and a name to index hierarchy map. // TODO make this nicer or something. (Just return a skeleton?)
 * @param root aiNode to start from
 * @param bonesToFill is the list of bones to fill
 * @param transformationsToFill is the list of transformations to fill
 * @param nameToIndex is the map to fill with name to index mapping
 * @return number of bones in the skeleton
 */
size_t fillSkeleton(aiNode* root, std::vector<BoneNode>& bonesToFill, std::vector<glm::mat4>& transformationsToFill, std::map<std::string, uint32_t>& nameToIndex)
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
		transformationsToFill.push_back(aiToGlm(currentNode->mTransformation));

		// add children to queue if any
		uint32_t numChildren = currentNode->mNumChildren;
		int parentIndex = nameToIndex[std::string(currentNode->mName.C_Str())];
		if (0 < numChildren)
		{
			for (uint32_t i = 0; i < numChildren; i++)
			{
				// create a bone node
				BoneNode childNode{};
				childNode.parent = parentIndex;
				std::string childName = std::string(currentNode->mChildren[i]->mName.C_Str());
				if (childName.find("Ctrl") != std::string::npos) // do not add control nodes
				{
					continue;
				}
				// set children of parent
				bonesToFill[parentIndex].children.push_back(boneCount);
				// map name to index
				nameToIndex[childName] = boneCount;
				// add the bone of the child to the list
				bonesToFill.push_back(childNode);
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
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PopulateArmatureData);

	for (unsigned int mi = 0; mi < scene->mNumMeshes; mi++)
	{
		GraphicsAsset asset{};
		aiMesh* mesh = scene->mMeshes[mi];

		std::vector<FullVertex> vertices(mesh->mNumVertices);
		std::vector<uint32_t> indices;
		asset.attributes = VertexAttributeFlags::FLAG_NONE;
		// check features
		if (mesh->HasPositions())
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_POSITION;
		if (mesh->HasNormals())
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_NORMAL;
		if (mesh->GetNumColorChannels() > 0)
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_COLOR;
		if (mesh->HasTextureCoords(0))
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_TEXCOORD;
		if (mesh->HasTangentsAndBitangents())
		{
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_TANGENT;
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_BITANGENT;
		}
		if (mesh->HasBones())
		{
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_JOINTS;
			asset.attributes = asset.attributes | VertexAttributeFlags::FLAG_WEIGHTS;
			// handle conversion of bone data to format that can be looped over
			std::map<std::string, uint32_t> nameToIndex;
			std::vector<BoneNode> bones;
			std::vector<glm::mat4> boneTransformations;
			// trust assimp for doing the work for us
			aiBone* firstBone = mesh->mBones[0];
			size_t numBones = fillSkeleton(firstBone->mArmature, bones, boneTransformations, nameToIndex);
			const Skeleton skeleton{ boneTransformations, bones };
			asset.skeleton.emplace(skeleton);
			// The following vector tells how many bones are registered per vertex. (max 4)
			std::vector<uint32_t> bonesUsed(vertices.size(), 0);
			for (uint32_t bi = 0u; bi < mesh->mNumBones; bi++)
			{
				aiBone* bone = mesh->mBones[bi];
				std::string boneName = std::string(bone->mName.C_Str());
				uint32_t boneIndex = nameToIndex[boneName];
				for (uint32_t wi = 0u; wi < bone->mNumWeights; wi++)
				{
					aiVertexWeight weight = bone->mWeights[wi];
					uint32_t vertexIndex = weight.mVertexId;
					uint32_t boneCount = bonesUsed[vertexIndex];
					if (boneCount < 4u)
					{
						vertices[vertexIndex].joints[boneCount] = boneIndex;
						vertices[vertexIndex].weights[boneCount] = weight.mWeight;
						bonesUsed[vertexIndex]++;
					}
				}
			}


			// load animations
			loadAnimations(scene, nameToIndex, asset.animations);
		} // end of if for bones



		// loop vertices
		for (uint32_t vi = 0u; vi < mesh->mNumVertices; vi++)
		{
			vertices[vi].position = aiToGlm(mesh->mVertices[vi]);

			if (mesh->HasNormals())
				vertices[vi].normal = aiToGlm(mesh->mNormals[vi]);
			if (mesh->HasVertexColors(0))
				vertices[vi].color = aiToGlm(mesh->mColors[0][vi]);
			if (mesh->HasTextureCoords(0))
				vertices[vi].texCoord = aiToGlm(mesh->mTextureCoords[0][vi]);
			if (mesh->HasTangentsAndBitangents())
			{
				vertices[vi].tangent = aiToGlm(mesh->mTangents[vi]);
				vertices[vi].bitangent = aiToGlm(mesh->mBitangents[vi]);
			}
		} // end of vertices for;

		for (uint32_t fi = 0u; fi < mesh->mNumFaces; fi++)
		{
			aiFace face = mesh->mFaces[fi];
			for (uint32_t i = 0u; i < face.mNumIndices; i++)
			{
				indices.push_back(face.mIndices[i]);
			}
		} // end of faces for


		// finalize the asset
		asset.vertices = vertices;
		asset.indices = indices;
		assets.push_back(asset);
	} // end of mesh for

	return assets;
}
