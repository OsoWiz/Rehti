#pragma once
#include <GraphicsResources.hpp>
#include <GraphicsTypesInternal.hpp>

/**
 * @brief GraphicsResourceManager is used for resource management.
 * It creates a vmaAllocator and all resource allocations to the gpu should be dispatched through this class.
*/
class GraphicsResourceManager
{
public:
	GraphicsResourceManager(uint32_t vkVersion, VkDevice& logDevice, VkPhysicalDevice& gpu, QueueDetails queueDetails);
	~GraphicsResourceManager();

	Buffer createBuffer(VkDeviceSize size, VkBufferUsageFlags flags, ResourceAllocationDetails allocationDetails = ResourceAllocationDetails(), VmaAllocationInfo* pAllocInfo = nullptr);
	// Todo more options for specifying image creation, such as mip levels, array layers, etc.
	Image createImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
						VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT,
						VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
						ResourceAllocationDetails allocationDetails = ResourceAllocationDetails(), VmaAllocationInfo* pAllocInfo = nullptr);
	// Todo image view creation while referencing a image resoure. There can be multiple view for a single image.

    struct TransitionDetails
	{
		VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
		VkAccessFlags2 srcAccess = 0;
		VkAccessFlags2 dstAccess = 0;
		VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT;
		uint32_t baseMipLevel = 0;
		uint32_t levelCount = 1;
		uint32_t baseArrayLayer = 0;
		uint32_t layerCount = 1;
	};
	// Image layout can be part of existing command buffer. If no command buffer is provided, a new one will be created and submitted immediately.
	void transitionImageLayout(Image& image, const TransitionDetails& transition, VkCommandBuffer existingCommand = VK_NULL_HANDLE);

	struct ImageCopyDetails
	{
		VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VkPipelineStageFlags2 finalStage = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		VkAccessFlags2 finalAccess = VK_ACCESS_2_SHADER_READ_BIT;
		VkImageAspectFlags aspects = VK_IMAGE_ASPECT_COLOR_BIT;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t baseMipLevel = 0;
		uint32_t levelCount = 1;
		uint32_t baseArrayLayer = 0;
		uint32_t layerCount = 1;
	};
	void copyToImage(Image& image, const void* data, VkDeviceSize dataSize, const ImageCopyDetails& details = ImageCopyDetails());

	void copyToBuffer(Buffer& buffer, const void* data);

	void destroyBuffer(Buffer& buffer);
	void destroyImage(Image& image);

private:
	VkCommandBuffer beginCommand();
	void endCommand(VkCommandBuffer commandBuffer, VkFence fence = VK_NULL_HANDLE);
	Buffer createStagingBuffer(VkDeviceSize size); // helper for creating staging buffers for transfers
	static TransitionDetails selectTransition(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspects);

	VmaAllocator allocator;
	VkDevice& logDevice;
	VkPhysicalDevice& gpu;
	VkQueue& queue;
	VkCommandPool commandPool;
};

