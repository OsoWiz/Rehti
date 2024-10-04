#include "DescriptorBuilder.h"

#include <algorithm>
#include <functional>
#include <stdexcept>

PoolManager::PoolManager(VkDevice device)
	: logDevice(device), currentPool(VK_NULL_HANDLE)
{
}

PoolManager::~PoolManager()
{
	for (auto& pool : usedPools)
	{
		vkDestroyDescriptorPool(logDevice, pool, nullptr);
	}

	for (auto& pool : freePools)
	{
		vkDestroyDescriptorPool(logDevice, pool, nullptr);
	}
}

void PoolManager::resetPools()
{
	for (auto pool : usedPools)
	{
		// Fun fact: this function signature has flags, but the spec states that they are for future use.
		vkResetDescriptorPool(logDevice, pool, 0);
		freePools.push_back(pool);
	}
	usedPools.clear();
	currentPool = VK_NULL_HANDLE;
}

bool PoolManager::allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& descSet)
{
	// Make sure we have a pool
	if (currentPool == VK_NULL_HANDLE)
	{
		currentPool = grabPool();
		usedPools.push_back(currentPool);
	}

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = currentPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout;

	// Try alloc
	VkResult allocResult = vkAllocateDescriptorSets(logDevice, &allocInfo, &descSet);
	bool realloc = false;

	switch (allocResult)
	{
		case VK_SUCCESS:
			return true;
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			realloc = true;
			break;
		default:
			return false;
	}

	if (realloc)
	{
		// Grab new pool and retry
		currentPool = grabPool();
		usedPools.push_back(currentPool);
		allocInfo.descriptorPool = currentPool;
		allocResult = vkAllocateDescriptorSets(logDevice, &allocInfo, &descSet);
		if (allocResult == VK_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

VkDescriptorPool PoolManager::createPool(VkDevice device, VkDescriptorPoolCreateFlags flags, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes)
{
	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.flags = flags;
	createInfo.maxSets = maxSets;
	createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	createInfo.pPoolSizes = poolSizes.data();

	VkDescriptorPool pool;
	if (vkCreateDescriptorPool(device, &createInfo, nullptr, &pool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool");
	}
	freePools.push_back(pool);

	return pool;
}

VkDescriptorPool PoolManager::grabPool()
{
	VkDescriptorPool pool;
	if (freePools.empty())
	{
		pool = createPool(logDevice, 0, 1000, STANDARD_POOL_SIZES);
	}
	else
	{
		auto lastDescpool = freePools.back();
		freePools.pop_back();
		pool = lastDescpool;
	}
	return pool;
}

DescriptorSetLayoutCache::DescriptorSetLayoutCache(VkDevice device)
	: logDevice(device)
{
}

DescriptorSetLayoutCache::~DescriptorSetLayoutCache()
{
	for (auto& layout : layoutCache)
	{
		vkDestroyDescriptorSetLayout(logDevice, layout.second, nullptr);
	}
}

VkDescriptorSetLayout DescriptorSetLayoutCache::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo& layoutInfo)
{
	DescriptorSetLayoutInfo info{};
	info.bindings.reserve(layoutInfo.bindingCount);

	bool sorted = true;
	int lastBinding = -1;

	for (uint32_t i = 0; i < layoutInfo.bindingCount; i++)
	{
		VkDescriptorSetLayoutBinding binding = layoutInfo.pBindings[i];
		info.bindings.push_back(binding);

		if (lastBinding < binding.binding) // check order
		{
			sorted = false;
		}
		lastBinding = static_cast<int>(binding.binding);
	}

	if (!sorted)
		std::sort(info.bindings.begin(), info.bindings.end(), [](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
			{
				return a.binding < b.binding;
			});
	// Info is now sanitized and sorted
	VkDescriptorSetLayout layout;

	auto layoutIt = layoutCache.find(info);
	if (layoutIt != layoutCache.end())
	{ // found
		return layoutIt->second;
	}
	else
	{ // not found
		if (vkCreateDescriptorSetLayout(logDevice, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout");
		}
		layoutCache[info] = layout;
	}
	return layout;
}

VkDescriptorSetLayout DescriptorSetLayoutCache::createDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindingCount);
	layoutInfo.pBindings = bindings;

	return createDescriptorSetLayout(layoutInfo);
}

bool DescriptorSetLayoutCache::DescriptorSetLayoutInfo::operator==(const DescriptorSetLayoutInfo& other) const
{
	if (bindings.size() != other.bindings.size())
	{
		return false;
	}
	for (uint32_t i = 0; i < bindings.size(); i++)
	{
		VkDescriptorSetLayoutBinding bindingThis = bindings[i];
		VkDescriptorSetLayoutBinding bindingThat = other.bindings[i];

		if (bindingThis.binding != bindingThat.binding ||
			bindingThis.descriptorCount != bindingThat.descriptorCount ||
			bindingThis.descriptorType != bindingThat.descriptorType ||
			bindingThis.stageFlags != bindingThat.stageFlags)
		{
			return false;
		}
	}

	return true;
}

size_t DescriptorSetLayoutCache::DescriptorSetLayoutInfo::hash() const
{
	size_t res = std::hash<size_t>()(bindings.size());

	for (const VkDescriptorSetLayoutBinding& bind : bindings)
	{
		size_t byteSize = sizeof(char);
		size_t bindHash = bind.binding | bind.descriptorCount << 8 | bind.descriptorType << 16 | bind.stageFlags << 24;
		res ^= std::hash<size_t>()(bindHash);
	}
	return res;
}

DescriptorBuilder::DescriptorBuilder(PoolManager& poolManager, DescriptorSetLayoutCache& cache)
	: currentBinding(0)
{
	pPoolManager = std::make_unique<PoolManager>(poolManager);
	pLayoutCache = std::make_unique<DescriptorSetLayoutCache>(cache);
}

DescriptorBuilder::~DescriptorBuilder()
{
}

DescriptorBuilder& DescriptorBuilder::bindBuffer(VkDescriptorBufferInfo& bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding newLayoutBinding = {};

	newLayoutBinding.descriptorCount = 1;
	newLayoutBinding.descriptorType = type;
	newLayoutBinding.stageFlags = stageFlags;
	newLayoutBinding.pImmutableSamplers = nullptr;
	newLayoutBinding.binding = currentBinding;

	layoutBindings.push_back(newLayoutBinding);

	VkWriteDescriptorSet newWriteSet = {};
	newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	newWriteSet.pNext = nullptr;
	newWriteSet.descriptorCount = 1;
	newWriteSet.descriptorType = type;
	newWriteSet.dstBinding = currentBinding; // naturally, layout binding, and the write set binding should match.
	newWriteSet.pBufferInfo = &bufferInfo;

	writeSets.push_back(newWriteSet);
	currentBinding++; // Set the next binding to be used
	return *this;
}

DescriptorBuilder& DescriptorBuilder::bindBuffers(const VkDescriptorBufferInfo* bufferInfos, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count)
{
	VkDescriptorSetLayoutBinding newLayoutBinding = {};

	newLayoutBinding.descriptorCount = count;
	newLayoutBinding.descriptorType = type;
	newLayoutBinding.stageFlags = stageFlags;
	newLayoutBinding.pImmutableSamplers = nullptr;
	newLayoutBinding.binding = currentBinding;

	layoutBindings.push_back(newLayoutBinding);

	VkWriteDescriptorSet newWriteSet = {};
	newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	newWriteSet.pNext = nullptr;
	newWriteSet.descriptorCount = count;
	newWriteSet.descriptorType = type;
	newWriteSet.dstBinding = currentBinding; // naturally, layout binding, and the write set binding should match.
	newWriteSet.pBufferInfo = bufferInfos;

	writeSets.push_back(newWriteSet);
	currentBinding++; // Set the next binding to be used
	return *this;
}

DescriptorBuilder& DescriptorBuilder::bindImage(VkDescriptorImageInfo& imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding newLayoutBinding = {};

	newLayoutBinding.descriptorCount = 1;
	newLayoutBinding.descriptorType = type;
	newLayoutBinding.stageFlags = stageFlags;
	newLayoutBinding.pImmutableSamplers = nullptr;
	newLayoutBinding.binding = currentBinding;

	layoutBindings.push_back(newLayoutBinding);

	VkWriteDescriptorSet newWriteSet = {};
	newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	newWriteSet.pNext = nullptr;
	newWriteSet.descriptorCount = 1;
	newWriteSet.descriptorType = type;
	newWriteSet.dstBinding = currentBinding;
	newWriteSet.pImageInfo = &imageInfo;
	// Dst set is not set yet, but will be at build time

	writeSets.push_back(newWriteSet);
	currentBinding++; // Set the next binding to be used
	return *this;
}

DescriptorBuilder& DescriptorBuilder::bindImages(const VkDescriptorImageInfo* imageInfos, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count)
{
	VkDescriptorSetLayoutBinding newLayoutBinding = {};

	newLayoutBinding.descriptorCount = count;
	newLayoutBinding.descriptorType = type;
	newLayoutBinding.stageFlags = stageFlags;
	newLayoutBinding.pImmutableSamplers = nullptr;
	newLayoutBinding.binding = currentBinding;

	layoutBindings.push_back(newLayoutBinding);

	VkWriteDescriptorSet newWriteSet = {};
	newWriteSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	newWriteSet.pNext = nullptr;
	newWriteSet.descriptorCount = count;
	newWriteSet.descriptorType = type;
	newWriteSet.dstBinding = currentBinding;
	newWriteSet.pImageInfo = imageInfos;
	// Dst set is not set yet, but will be at build time

	writeSets.push_back(newWriteSet);
	currentBinding++; // Set the next binding to be used
	return *this;
}

bool DescriptorBuilder::build(VkDescriptorSet& set, VkDescriptorSetLayout& layout)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();

	layout = pLayoutCache->createDescriptorSetLayout(layoutInfo); // caches the layout if created
	// Allocate the set
	if (!pPoolManager->allocateDescriptorSet(layout, set))
	{
		return false;
	}

	// And then handle the writes
	for (VkWriteDescriptorSet& write : writeSets)
	{
		write.dstSet = set;
	}
	// updates the set
	vkUpdateDescriptorSets(pPoolManager->getDevice(), static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, nullptr);

	// Clear the vectors
	layoutBindings.clear();
	writeSets.clear();
	currentBinding = 0;

	return true;
}

bool DescriptorBuilder::build(VkDescriptorSet& set)
{
	VkDescriptorSetLayout layout;
	return build(set, layout);
}

void DescriptorBuilder::setDescriptorSetLayout(const VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount, VkDescriptorSetLayout& layout)
{
	layout = pLayoutCache->createDescriptorSetLayout(bindings, bindingCount);
}

const PoolManager& DescriptorBuilder::getPoolManager() const
{
	return *pPoolManager;
}