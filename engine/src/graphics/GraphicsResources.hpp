#pragma once
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <utility>

/*
* This file contains various resources that are used to store data in GPU memory.
*/

struct ResourceAllocationDetails
{
	VmaMemoryUsage memUsage = VMA_MEMORY_USAGE_AUTO;
	VmaAllocationCreateFlags vmaCreationFlags = 0;
	VkMemoryPropertyFlags requiredFlags = 0;
	VkMemoryPropertyFlags preferredFlags = 0;
};

struct ImageDetails
{
	std::pair<uint32_t, uint32_t> extent{ 0, 0 };
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

struct ImageViewDetails
{
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT;
	uint32_t baseMipLevel = 0;
	uint32_t levelCount = 1;
};

/**
 * @brief Buffer is any data that is stored in the GPU memory.
 */
struct Buffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
};

/**
 * @brief Image consists of view, image and allocation.
 */
struct Image
{
	VkImage image;
	VkImageView view;
	VmaAllocation allocation;
	VkExtent3D extent{ 0, 0, 1 };
	VkFormat format = VK_FORMAT_UNDEFINED;
	VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
	uint32_t mipLevels = 1;
	uint32_t arrayLayers = 1;
};
