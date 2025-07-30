#include "GraphicsResources.hpp"

#define VMA_IMPLEMENTATION
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>


Buffer createBuffer(VmaAllocator& allocator, VkDeviceSize size, VkBufferUsageFlags flags)
{
	Buffer newBuffer{};
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = flags;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr);
	return newBuffer;
}

Image createImage(VmaAllocator& allocator, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags flags)
{
	Image newImage{};
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	vmaCreateImage(allocator, &imageInfo, &allocInfo, &newImage.image, &newImage.allocation, nullptr);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = newImage.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = flags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VmaAllocatorInfo allocatorInfo{};
	vmaGetAllocatorInfo(allocator, &allocatorInfo);
	vkCreateImageView(allocatorInfo.device, &viewInfo, nullptr, &newImage.view);

	return newImage;
}
