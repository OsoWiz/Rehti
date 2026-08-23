#pragma once

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <vector>
// Chunk size for descriptor pool allocations
constexpr VkDeviceSize POOL_CHUNK_SIZE = 256;

// Standard pool sizes for descriptor types
const std::vector<VkDescriptorPoolSize> STANDARD_POOL_SIZES({ {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
															 {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
															 {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
															 {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
															 {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
															 {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
															 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
															 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
															 {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
															 {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
															 {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} });

/**
*
* This class manages descriptor pools, and allows for the allocation of descriptor sets.
*/
class PoolManager
{
public:
	PoolManager(VkDevice device);
	~PoolManager();

   /**
	*
	* Resets pools
	*/
	void resetPools();

   /**
	*
	* Allocates a descriptor set from the pool
	* @param layout Layout to be used
	* @param descSet Descriptor set to be allocated
	* @return
	*/
	bool allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& descSet);

   /**
	*
	* Allocates a descriptor pool and adds it to the free pools.
	* @param device logical device
	* @param flags flags
	* @param maxSets maximum number of sets to be allocated from this pool
	* @param poolSizes List of limits on individual descriptors that can be allocated from this pool. For
	* example 2 uniform buffers or 3 images.
	* @return
	*/
	VkDescriptorPool createPool(VkDevice device, VkDescriptorPoolCreateFlags flags, uint32_t maxSets,
		const std::vector<VkDescriptorPoolSize>& poolSizes);

   /**
	*
	* Returns the logical device used by this manager.
	* @return
	*/
	VkDevice getDevice() const
	{
		return logDevice;
	}

private:
   /**
	*
	* Returns a pool from free pools or creates a new one.
	* Does not push the returned pool to used pools.
	* @return
	*/
	VkDescriptorPool grabPool();

	VkDevice logDevice;
	VkDescriptorPool currentPool;
	std::vector<VkDescriptorPool> freePools;
	std::vector<VkDescriptorPool> usedPools;
};

/**
*
* The use of this class is to cache descriptor set layouts.
*/
class DescriptorSetLayoutCache
{
public:
	DescriptorSetLayoutCache(VkDevice device);
	~DescriptorSetLayoutCache();

   /**
	*
	* Creates a descriptor set layout from the given info, or returns one from the cache if it already exists.
	* @param layoutInfo Layout info
	* @return Allocated descriptor set layout
	*/
	VkDescriptorSetLayout createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& layoutInfo);

   /**
	*
	* Creates a descriptor set layout from the given bindings, or returns one from the cache if it already exists.
	* @param bindings to create a layout of.
	* @param bindingCount of the bindings.
	* @return Created layout.
	*/
	VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings,
		uint32_t bindingCount);

	struct DescriptorSetLayoutInfo
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		bool operator==(const DescriptorSetLayoutInfo& other) const;
		size_t hash() const;
	};

private:
	struct DescriptorLayoutHasher
	{
		size_t operator()(const DescriptorSetLayoutInfo& info) const
		{
			return info.hash();
		}
	};

	VkDevice logDevice;
	std::unordered_map<DescriptorSetLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHasher> layoutCache;
};

/**
*
* DescriptorBuilder uses PoolManager and DescriptorLayoutCache to manage and create descriptor sets.
*/
class DescriptorBuilder
{
public:
	DescriptorBuilder(VkDevice device);
	~DescriptorBuilder();

   /**
	*
	* Creates a descriptor set layout binding and a write descriptor set for a buffer.
	* @param bufferInfo struct describing the buffer data.
	* @param type of descriptor
	* @param stageFlags Descriptor stage
	* @return The builder itself.
	*/
	DescriptorBuilder& bindBuffer(VkDescriptorBufferInfo& bufferInfo, VkDescriptorType type,
		VkShaderStageFlags stageFlags);

	/**
	 * @brief Creates a descriptor set layout binding and a write descriptor set for multiple buffers.
	 * @param bufferInfos is a pointer to the list of buffer info structs.
	 * @param type is the type of resource.
	 * @param stageFlags is the shader stage.
	 * @param count is the number of elements in the bufferInfos array.
	 * @return The builder itself
	 */
	DescriptorBuilder& bindBuffers(const VkDescriptorBufferInfo* bufferInfos, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count);

   /**
	*
	* Creates a descriptor set layout binding and a write descriptor set for an image.
	* @param imageInfo struct decribing the image data.
	* @param type of descriptor.
	* @param stageFlags Descriptor stage.
	* @return The builder itself.
	*/
	DescriptorBuilder& bindImage(VkDescriptorImageInfo& imageInfo, VkDescriptorType type,
		VkShaderStageFlags stageFlags);

	/**
	 * @brief Binds multiple images to the descriptor set as an array.
	 * @param imageInfos is an array of image info structs.
	 * @param type of resource.
	 * @param stageFlags is the shader stage.
	 * @param count is the number of images to bind. Must match what is in the imageInfos array.
	 * @return the builder itself.
	 */
	DescriptorBuilder& bindImages(const VkDescriptorImageInfo* imageInfos, VkDescriptorType type,
		VkShaderStageFlags stageFlags, uint32_t count);

   /**
	*
	* Builds a descriptor set and sets a created layout to the supplied parameter.
	* Also updates the descriptor sets.
	* @param set to be created.
	* @param layout to be filled.
	* @return Boolean indicating operation status.
	*/
	bool build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);

   /**
	*
	* Builds a descriptor set.
	* @param set set to be built
	* @return Boolean indicating operation status
	*/
	bool build(VkDescriptorSet& set);

   /**
	*
	* Sets a layout to the supplied parameter from the given bindings.
	* @param layout to be set
	* @param bindings to be used
	*/
	void setDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount,
		VkDescriptorSetLayout& layout);


   /**
	*
	* Creates a descriptor set layout using builders layoutCache member.
	* @param layoutInfo Layout info
	* @return Allocated descriptor set layout
	*/
	VkDescriptorSetLayout createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& layoutInfo);

   /**
	*
	* Creates a descriptor set layout using builders layoutCache member
	* @param bindings to create a layout of.
	* @param bindingCount of the bindings.
	* @return Created layout.
	*/
	VkDescriptorSetLayout createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings,
		uint32_t bindingCount);

	const PoolManager& getPoolManager() const;

private:
	uint32_t currentBinding;
	std::vector<VkWriteDescriptorSet> writeSets;
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	std::unique_ptr<PoolManager> pPoolManager;
	std::unique_ptr<DescriptorSetLayoutCache> pLayoutCache;
};