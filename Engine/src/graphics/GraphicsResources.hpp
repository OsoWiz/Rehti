#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

/*
* This file contains various resources that are used to store data in GPU memory.
*/

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
};


Buffer createBuffer(VmaAllocator& allocator, VkDeviceSize size, VkBufferUsageFlags flags);

Image createImage(VmaAllocator& allocator, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags flags);
