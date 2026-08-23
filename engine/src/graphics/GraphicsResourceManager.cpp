#include "GraphicsResourceManager.hpp"
#include "RehtiException.hpp"
#include <Logger.hpp>
#include <cassert>
#include <cstring>

#define VMA_IMPLEMENTATION
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

GraphicsResourceManager::GraphicsResourceManager(uint32_t vkVersion, VkInstance& instance, VkDevice& logDevice, VkPhysicalDevice& gpu, QueueDetails queueDetails)
	:logDevice(logDevice), gpu(gpu), queue(queueDetails.queue)
{
	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.vulkanApiVersion = vkVersion;
	allocatorInfo.instance = instance;
	allocatorInfo.device = logDevice;
	allocatorInfo.physicalDevice = gpu;

	if (vmaCreateAllocator(&allocatorInfo, &allocator) != VK_SUCCESS)
	{
		throw RehtiException(RehtiError::INITIALIZATION_FAILURE, "Failed to create a VMA in graphics resource manager.");
	}

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueDetails.familyIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	if (vkCreateCommandPool(logDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw RehtiException(RehtiError::INITIALIZATION_FAILURE, "Failed to create a command pool in graphics resource manager.");
	}
}

GraphicsResourceManager::~GraphicsResourceManager()
{}

Buffer GraphicsResourceManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags flags, ResourceAllocationDetails allocationDetails, VmaAllocationInfo* pAllocInfo)
{
	Buffer newBuffer{};
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = flags;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, pAllocInfo);
	return newBuffer;
}

Image GraphicsResourceManager::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspects, VkImageTiling tiling, ResourceAllocationDetails allocationDetails, VmaAllocationInfo* pAllocInfo)
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
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	vmaCreateImage(allocator, &imageInfo, &allocInfo, &newImage.image, &newImage.allocation, pAllocInfo);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = newImage.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspects;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VmaAllocatorInfo allocatorInfo{};
	vmaGetAllocatorInfo(allocator, &allocatorInfo);
	vkCreateImageView(logDevice, &viewInfo, nullptr, &newImage.view);

	newImage.extent = imageInfo.extent;
	newImage.format = format;
	newImage.aspects = aspects;
	newImage.layout = imageInfo.initialLayout;
	newImage.mipLevels = imageInfo.mipLevels;
	newImage.arrayLayers = imageInfo.arrayLayers;

	return newImage;
}

void GraphicsResourceManager::transitionImageLayout(Image& image, const TransitionDetails& transition, VkCommandBuffer existingCommand)
{
	bool newCmdBufferRequired = existingCommand == VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer = existingCommand;
	if (newCmdBufferRequired)
	{
		commandBuffer = beginCommand();
	}
 VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.srcStageMask = transition.srcStage;
	barrier.dstStageMask = transition.dstStage;
	barrier.srcAccessMask = transition.srcAccess;
	barrier.dstAccessMask = transition.dstAccess;
	barrier.oldLayout = transition.oldLayout;
	barrier.newLayout = transition.newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // todo add this when you want to transfer queue ownership as well.
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image.image;
	barrier.subresourceRange.aspectMask = transition.aspects;
	barrier.subresourceRange.baseMipLevel = transition.baseMipLevel;
	barrier.subresourceRange.levelCount = transition.levelCount;
	barrier.subresourceRange.baseArrayLayer = transition.baseArrayLayer;
	barrier.subresourceRange.layerCount = transition.layerCount;

	VkDependencyInfo dependencyInfo{};
	dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

	if (newCmdBufferRequired)
	{
		endCommand(commandBuffer);
	}

	image.layout = transition.newLayout;
}

void GraphicsResourceManager::copyToBuffer(Buffer& buffer, const void* data)
{
	assert(buffer.buffer != VK_NULL_HANDLE || data != nullptr);
	
	VkMemoryPropertyFlags props{};
	VmaAllocationInfo allocInfo{};
	vmaGetAllocationMemoryProperties(allocator, buffer.allocation, &props);
	vmaGetAllocationInfo(allocator, buffer.allocation, &allocInfo);
	VkDeviceSize bufferSize = allocInfo.size;

	if (props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		void* mappedData;
		vmaMapMemory(allocator, buffer.allocation, &mappedData);
		std::memcpy(mappedData, data, static_cast<size_t>(bufferSize));
		
       if (!(props & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) 
		{
			vmaFlushAllocation(allocator, buffer.allocation, 0, bufferSize);
		}

		vmaUnmapMemory(allocator, buffer.allocation);
	}
	else
	{
		Buffer stagingBuffer = createStagingBuffer(bufferSize);
		void* mappedData;
		vmaMapMemory(allocator, stagingBuffer.allocation, &mappedData);
		std::memcpy(mappedData, data, static_cast<size_t>(bufferSize));
		vmaUnmapMemory(allocator, stagingBuffer.allocation);
		VkCommandBuffer commandBuffer = beginCommand();
		
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = bufferSize;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, buffer.buffer, 1, &copyRegion);
		endCommand(commandBuffer);
		destroyBuffer(stagingBuffer);
	}
}

void GraphicsResourceManager::copyToImage(Image& image, const void* data, VkDeviceSize dataSize, const ImageCopyDetails& details)
{
   assert(image.image != VK_NULL_HANDLE && data != nullptr);

	uint32_t width = details.width != 0 ? details.width : image.extent.width;
	uint32_t height = details.height != 0 ? details.height : image.extent.height;
	VkDeviceSize uploadSize = dataSize;
	if (uploadSize == 0)
	{
		uploadSize = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * 4;
	}

	Buffer stagingBuffer = createStagingBuffer(uploadSize);
	void* mappedData;
	vmaMapMemory(allocator, stagingBuffer.allocation, &mappedData);
	std::memcpy(mappedData, data, static_cast<size_t>(uploadSize));
	vmaUnmapMemory(allocator, stagingBuffer.allocation);

	VkCommandBuffer commandBuffer = beginCommand();

	TransitionDetails toTransfer = selectTransition(image.layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, details.aspects);
	toTransfer.baseMipLevel = details.baseMipLevel;
	toTransfer.levelCount = details.levelCount;
	toTransfer.baseArrayLayer = details.baseArrayLayer;
	toTransfer.layerCount = details.layerCount;
	transitionImageLayout(image, toTransfer, commandBuffer);

	VkBufferImageCopy copyRegion{};
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;
	copyRegion.imageSubresource.aspectMask = details.aspects;
	copyRegion.imageSubresource.mipLevel = details.baseMipLevel;
	copyRegion.imageSubresource.baseArrayLayer = details.baseArrayLayer;
	copyRegion.imageSubresource.layerCount = details.layerCount;
	copyRegion.imageOffset = { 0, 0, 0 };
    copyRegion.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

	TransitionDetails toFinal = selectTransition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, details.finalLayout, details.aspects);
	toFinal.baseMipLevel = details.baseMipLevel;
	toFinal.levelCount = details.levelCount;
	toFinal.baseArrayLayer = details.baseArrayLayer;
	toFinal.layerCount = details.layerCount;
	toFinal.srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	toFinal.srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	toFinal.dstStage = details.finalStage;
	toFinal.dstAccess = details.finalAccess;
	transitionImageLayout(image, toFinal, commandBuffer);

	endCommand(commandBuffer);
	destroyBuffer(stagingBuffer);
}

void GraphicsResourceManager::destroyBuffer(Buffer& buffer)
{
	vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
}

void GraphicsResourceManager::destroyImage(Image& image)
{
	vkDestroyImageView(logDevice, image.view, nullptr);
	vmaDestroyImage(allocator, image.image, image.allocation);
}

VkCommandBuffer GraphicsResourceManager::beginCommand()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	
	VkCommandBuffer commandBuffer{};
	if (vkAllocateCommandBuffers(logDevice, &allocInfo, &commandBuffer) != VK_SUCCESS)
	{
		throw RehtiException(RehtiError::INITIALIZATION_FAILURE, "Failed to allocate command buffer in graphics resource manager.");
	}
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (VK_SUCCESS != vkBeginCommandBuffer(commandBuffer, &beginInfo))
	{
		throw RehtiException(RehtiError::GRAPHICS_GENERIC_ERROR, "Failed to begin command buffer in graphics resource manager.");
	}

	return commandBuffer;
}

void GraphicsResourceManager::endCommand(VkCommandBuffer commandBuffer, VkFence fence)
{
	if (VK_SUCCESS != vkEndCommandBuffer(commandBuffer))
	{
		throw RehtiException(RehtiError::GRAPHICS_GENERIC_ERROR, "Failed to end command buffer in graphics resource manager.");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, fence);

	if (fence == VK_NULL_HANDLE) // No fence, idle
	{
		vkQueueWaitIdle(queue);
	}

	vkFreeCommandBuffers(logDevice, commandPool, 1, &commandBuffer);
}

Buffer GraphicsResourceManager::createStagingBuffer(VkDeviceSize size)
{
	ResourceAllocationDetails stagingAllocDetails{};
	stagingAllocDetails.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	stagingAllocDetails.vmaCreationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
	return createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingAllocDetails, nullptr);
}

GraphicsResourceManager::TransitionDetails GraphicsResourceManager::selectTransition(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspects)
{
	TransitionDetails details{};
	details.oldLayout = oldLayout;
	details.newLayout = newLayout;
	details.aspects = aspects;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
       details.srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		details.dstStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		details.srcAccess = 0;
       details.dstAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
      details.srcStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		details.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		details.srcAccess = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		details.dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
       details.srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		details.dstStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		details.srcAccess = 0;
      details.dstAccess = VK_ACCESS_2_SHADER_READ_BIT;
	}
	else
	{
      details.srcStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		details.dstStage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		details.srcAccess = 0;
		details.dstAccess = 0;
	}

	return details;
}
