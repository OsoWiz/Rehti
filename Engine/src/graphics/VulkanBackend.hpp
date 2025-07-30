#pragma once

#include "GraphicsResources.hpp"
#include <string>
#include <vector>

// FWD decl
struct SDL_Window;
struct Image;

// decl

constexpr uint32_t REQUIRED_VULKAN_VERSION = VK_API_VERSION_1_3;

enum class VulkanBackendFlags : uint16_t
{
	NONE,
	DYNAMIC_RENDERING = 1 << 0, // use dynamic rendering instead of render passes
};


class VulkanBackend
{
public:
	//static void initialize(const GraphicsSettings& settings);
	//static void cleanup();

private:
	VulkanBackend() = default;
	VulkanBackend(const VulkanBackend&) = delete;
	VulkanBackend& operator=(const VulkanBackend&) = delete;

	SDL_Window* window = nullptr;

	// Vulkan structures
	VkInstance instance = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkPhysicalDevice physDevice = VK_NULL_HANDLE;
	VkDevice logDevice = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> swapChainImages = std::vector<VkImage>();
	std::vector<VkImageView> swapChainImageViews = std::vector<VkImageView>();
	std::vector<VkFramebuffer> frameBuffers = std::vector<VkFramebuffer>();
	std::vector<VkCommandBuffer> commandBuffers = std::vector<VkCommandBuffer>();
	std::vector<const char*> instanceExtensions = std::vector<const char*>();
	std::vector<const char*> deviceExtensions = std::vector<const char*>();
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkFormat depthFormat;
	Image depthImage;
	VkCommandPool commandPool = VK_NULL_HANDLE;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	// PipelineManager pipelineManager;

	VmaAllocator gpuAllocator;

	// private functions
	void createInstance();
	void setupDebugging();
	void eventLoop(); // TODO move to another class. event looping should be handled separately.
	void createSurface();
	void createPhysicalDevice();
	void createLogicalDevice();
	void createAllocator();
	void createSwapChain();
	void createImageViews();
	void createDepthResources();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSynchronization();

	void initialize(const GraphicsSettings& graphicsSettings);
	void cleanup();
};

