#include "RehtiGraphics.hpp"
#include "ShaderTools.hpp"
#include "GraphicsUtils.hpp"
#include "GraphicsResourceManager.hpp"
#include "Camera.hpp"
#include "PipelineManager.hpp"
#include "RehtiException.hpp"
#include "Logger.hpp"
#include <flecs.h>
#include <iostream>

// constants
constexpr uint32_t REQUIRED_VULKAN_VERSION = VK_API_VERSION_1_3;
constexpr uint32_t NUM_BUFFERED_FRAMES = 2;

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <set>
#include <stack>
#include <vector>
#include <cassert>
#include <stdexcept>


#ifdef NDEBUG
static bool debug = false;
#else
static bool debug = true;
#endif // !

// error checks
#define VK_CHECK(x, msg) if (x != VK_SUCCESS) { throw std::runtime_error(msg); }
#define VK_NULL_CHECK(x, msg) if (x == VK_NULL_HANDLE) { throw std::runtime_error(msg); }
#define SDL_CHECK(x, msg) if (x != true) { throw std::runtime_error(msg); }
#define NULL_CHECK(x, msg) if (x == nullptr) { throw std::runtime_error(msg); }

struct InstanceSettings
{
	std::vector<const char*> layers;
	std::vector<const char*> extensions;
};

struct GraphicsConstants
{
	constexpr static VkClearColorValue CLEAR_COLOR { 0.0f, 0.0f, 0.0f, 1.0f };
};

struct RehtiGraphics::Backend
{
	// decl
	// Members
	~Backend() = default; // manual cleanup.

	void initialize(const Settings& graphicsSettings);
	void cleanup();

	SDL_Window* window = nullptr;
	flecs::world& world;
	// Vulkan structures
	uint32_t vulkanVersion = 0u;
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

	// cleanup struct
	std::stack<std::function<void()>> cleanupStack;

	std::unique_ptr <ShaderTools> shaderTools;
	std::unique_ptr <PipelineManager> pipelineManager;
    std::unique_ptr<GraphicsResourceManager> resourceManager;

	flecs::query<> drawQuery;

	// setup functions
	void createInstance();
	void setupDebugging();
	void eventLoop(); // TODO move to another class. event looping should be handled separately.
	void createSurface();
	void createPhysicalDevice();
	void createLogicalDevice();
	void createGraphicsResourceManager();
	void createPipelineManager();
	void createShaderTools();
	void createSwapChain();
	void createImageViews();
	void createDepthResources();
	// Todo make a compile time config for choosing between renderpass and dynamic?
	void createRenderPass();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSynchronization();

	void createDrawQuery();

	void recordDrawCommands();

	uint32_t currentFrame = 0;
};

void RehtiGraphics::Backend::setupDebugging()
{
	if (!debug)
		return; // Validationlayers are not enabled. Go back
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
	cleanupStack.push(std::bind(DestroyDebugUtilsMessengerEXT, instance, debugMessenger, nullptr));
}

/**
 * @brief helper to get the camera push constant range.
 * @return VkPushConstantRange
 */
VkPushConstantRange getCameraRange()
{
	VkPushConstantRange range{};
	range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	range.offset = 0;
	range.size = sizeof(glm::mat4);
	return range;
}

/**
 * @brief Checks support for the requested layers
 * @param requestedLayers as a vector of const char* names
 * @return boolean indicating whether all requested layers were found
 */
bool checkLayers(std::vector<const char*>& requestedLayers)
{
	uint32_t count = 0;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	std::vector<VkLayerProperties> availableLayers(count);
	vkEnumerateInstanceLayerProperties(&count, availableLayers.data());
	bool allFound = true;
	for (const char* layerName : requestedLayers)
	{
		bool found = false;
		for (const auto& layerProperties : availableLayers)
		{
			found = found || (!strcmp(layerName, layerProperties.layerName));
		}
		allFound = found && allFound;
	}
	return allFound;
}

std::vector<const char*> getLayers()
{
	std::vector<const char*> layers;
	if (debug)
	{
		layers.push_back("VK_LAYER_KHRONOS_validation");
	}
	return layers;
}


void RehtiGraphics::Backend::eventLoop()
{
	SDL_Event event;
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				quit = true;
			}
		}
	}
}

void RehtiGraphics::Backend::createInstance()
{
	VkApplicationInfo info{};
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationName = "RehtiGraphics";
	info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	info.pEngineName = "RehtiGraphics";
	info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	info.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &info;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();
	std::vector<const char* > layers = { "VK_LAYER_KHRONOS_validation" };
	if (!checkLayers(layers))
	{
		throw std::runtime_error("Requested layers not available");
	}
	// for some reason, using vcpkg installed validation layers causes access violation.
	createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	createInfo.ppEnabledLayerNames = layers.data();
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (debug)
	{
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.pNext = nullptr;
	}

	VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance), "Could not create Vulkan instance");
	cleanupStack.push(std::bind(vkDestroyInstance, instance, nullptr));
}

void RehtiGraphics::Backend::createSurface()
{
	SDL_CHECK(SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface), "Unable to create a vulkan surface.");
	cleanupStack.push([this]()
		{
			if (surface != VK_NULL_HANDLE)
			{
				vkDestroySurfaceKHR(instance, surface, nullptr);
				surface = VK_NULL_HANDLE;
			}
		});
}

inline int getGPUScore(const VkPhysicalDeviceProperties& props)
{
	int score = 0;
	switch (props.deviceType)
	{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score += 1000;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score += 50;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			score += 20;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			score += 1;
			break;
		default:
			break;
	}
	VkPhysicalDeviceLimits limits = props.limits;
	if (limits.maxPushConstantsSize > 128)
		score += 20;

	return score;
}

VkPhysicalDevice pickPhysicalDevice(std::vector<VkPhysicalDevice>& physDevices)
{
	assert(!physDevices.empty());

	std::sort(physDevices.begin(), physDevices.end(), [](const VkPhysicalDevice& a, const VkPhysicalDevice& b)
		{
			VkPhysicalDeviceProperties propsA, propsB;
			vkGetPhysicalDeviceProperties(a, &propsA);
			vkGetPhysicalDeviceProperties(b, &propsB);

			return getGPUScore(propsA) > getGPUScore(propsB);
		});
	return physDevices[0];
}

void RehtiGraphics::Backend::createPhysicalDevice()
{
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("No physical devices found");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	physDevice = pickPhysicalDevice(devices);
}

void RehtiGraphics::Backend::createLogicalDevice()
{
	QueueFamilyIndices indice = findQueueFamilies(this->physDevice, this->surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
	std::set<uint32_t> uniqueQueueFamilies = { indice.graphicsFamily.value(), indice.presentFamily.value() };
	if (indice.transferFamily.has_value())
	{
		uniqueQueueFamilies.insert(indice.transferFamily.value());
	}

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;

		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Check for anisotropy support
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo devCreateInfo{};
	devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	devCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

	devCreateInfo.pEnabledFeatures = &deviceFeatures;

	devCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	devCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	// usually enabled layercount is defined here
	// however since 1.3.289 this has not been necessary.
	devCreateInfo.enabledLayerCount = 0;

	VK_CHECK(vkCreateDevice(physDevice, &devCreateInfo, nullptr, &logDevice), "Could not create logical device");
	cleanupStack.push([this]()
		{
			if (logDevice != VK_NULL_HANDLE)
			{
				vkDestroyDevice(logDevice, nullptr);
				logDevice = VK_NULL_HANDLE;
			}
		});
	// TODO create a transfer queue as well
	vkGetDeviceQueue(logDevice, indice.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logDevice, indice.presentFamily.value(), 0, &presentQueue);
}

void RehtiGraphics::Backend::createGraphicsResourceManager()
{
	QueueDetails queueDetails{};
	queueDetails.familyIndex = findQueueFamilies(this->physDevice, this->surface).graphicsFamily.value();
	queueDetails.queue = graphicsQueue;
    this->resourceManager.reset(new GraphicsResourceManager(vulkanVersion, logDevice, physDevice, queueDetails));
}

void RehtiGraphics::Backend::createPipelineManager()
{
	this->pipelineManager.reset(new PipelineManager(logDevice));
}

void RehtiGraphics::Backend::createShaderTools()
{
	this->shaderTools.reset(new ShaderTools(logDevice));
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& format : availableFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;
	}

	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availableModes)
{
	for (const auto& presentmode : availableModes)
	{
		if (presentmode == VK_PRESENT_MODE_MAILBOX_KHR)
			return presentmode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(SDL_Window* window, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{ // special value
		int width, height;
		SDL_GetWindowSizeInPixels(window, &width, &height);

		VkExtent2D actual = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height) };

		actual.width = std::clamp(actual.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actual.height = std::clamp(actual.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actual;
	}
}

void RehtiGraphics::Backend::createSwapChain()
{
	SwapChainSupportDetails details = querySwapChainSupport(this->physDevice, this->surface);
	if (window != nullptr
		&& (details.formats.empty() || details.presentModes.empty()))
	{
		throw std::runtime_error("Swap chain support not available");
	}

	VkSurfaceFormatKHR format = chooseSwapSurfaceFormat(details.formats);
	VkPresentModeKHR mode = chooseSwapPresentMode(details.presentModes);
	VkExtent2D extent = chooseSwapExtent(this->window, details.capabilities);

	uint32_t imageCount = details.capabilities.minImageCount + 1; // for now, could choose smarter

	if (imageCount > details.capabilities.maxImageCount && details.capabilities.maxImageCount > 0) // 0 means "infinite". So > 0 means it has a limit
		imageCount = details.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR swapInfo{};
	swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapInfo.surface = surface;
	swapInfo.minImageCount = imageCount;
	swapInfo.imageFormat = format.format;
	swapInfo.imageColorSpace = format.colorSpace;
	swapInfo.imageExtent = extent;
	swapInfo.imageArrayLayers = 1; // Always 1 unless vr
	swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Onto conditional members
	QueueFamilyIndices indices = findQueueFamilies(this->physDevice, this->surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapInfo.queueFamilyIndexCount = 2;
		swapInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapInfo.queueFamilyIndexCount = 0;     // optionals
		swapInfo.pQueueFamilyIndices = nullptr; // optionals
	}

	// Some more
	swapInfo.preTransform = details.capabilities.currentTransform; // no transform
	swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;   // Defines how to use alpha channel with other windows.

	swapInfo.presentMode = mode;
	swapInfo.clipped = VK_TRUE;

	swapInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_CHECK(vkCreateSwapchainKHR(logDevice, &swapInfo, nullptr, &swapChain), "Failed to create a swapchain");

	vkGetSwapchainImagesKHR(logDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logDevice, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = format.format;
	swapChainExtent = extent;

	cleanupStack.push([this]()
		{
			if (swapChain != VK_NULL_HANDLE)
			{
				vkDestroySwapchainKHR(logDevice, swapChain, nullptr);
				swapChain = VK_NULL_HANDLE;
			}
			swapChainImages.clear();
		});
}

void RehtiGraphics::Backend::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageInfo.image = swapChainImages[i];
		imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageInfo.format = swapChainImageFormat;

		imageInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageInfo.subresourceRange.baseMipLevel = 0;
		imageInfo.subresourceRange.levelCount = 1;
		imageInfo.subresourceRange.baseArrayLayer = 0;
		imageInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(logDevice, &imageInfo, nullptr, &swapChainImageViews[i]), "Failed to create an image view");
	}

	cleanupStack.push([this]()
		{
			for (VkImageView imageView : swapChainImageViews)
			{
				if (imageView != VK_NULL_HANDLE)
				{
					vkDestroyImageView(logDevice, imageView, nullptr);
				}
			}
			swapChainImageViews.clear();
		});
}

VkFormat getDepthFormat(VkPhysicalDevice physDevice, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    std::vector<VkFormat> candidates = {
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT
	}; // todo make configurable
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}
	throw std::runtime_error("Failed to find supported format");
}

void RehtiGraphics::Backend::createDepthResources()
{
	depthFormat = getDepthFormat(this->physDevice, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	depthImage = resourceManager->createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	GraphicsResourceManager::TransitionDetails depthTransition = GraphicsResourceManager::TransitionDetails{};
	depthTransition.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthTransition.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthTransition.srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	depthTransition.dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	depthTransition.srcAccess = 0;
	depthTransition.dstAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	depthTransition.aspects = VK_IMAGE_ASPECT_DEPTH_BIT;
	resourceManager->transitionImageLayout(depthImage, depthTransition, VK_NULL_HANDLE);
	cleanupStack.push([this]()
		{

			if (depthImage.image != VK_NULL_HANDLE && depthImage.allocation != nullptr)
			{
				resourceManager->destroyImage(depthImage);
				depthImage.image = VK_NULL_HANDLE;
				depthImage.allocation = nullptr;
			}
		});
}

void RehtiGraphics::Backend::createRenderPass()
{
	VkAttachmentDescription colorattachment{};
	colorattachment.format = swapChainImageFormat;
	colorattachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorattachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorattachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorattachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorattachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorattachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorattachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachRef{};
	attachRef.attachment = 0;
	attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = depthFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachRef{};
	depthAttachRef.attachment = 1;
	depthAttachRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachRef;
	subpass.pDepthStencilAttachment = &depthAttachRef;

	VkSubpassDependency depend{};
	depend.srcSubpass = VK_SUBPASS_EXTERNAL;
	depend.dstSubpass = 0;
	depend.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	depend.srcAccessMask = 0;
	depend.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	depend.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorattachment, depthAttachment };
	VkRenderPassCreateInfo renderpassInfo{};
	
	renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderpassInfo.pAttachments = attachments.data();
	renderpassInfo.subpassCount = 1;
	renderpassInfo.pSubpasses = &subpass;
	renderpassInfo.dependencyCount = 1;
	renderpassInfo.pDependencies = &depend;

	VK_CHECK(vkCreateRenderPass(logDevice, &renderpassInfo, nullptr, &renderPass), "failed to create a renderpass");
	cleanupStack.push([this]()
		{
			if (renderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(logDevice, renderPass, nullptr);
				renderPass = VK_NULL_HANDLE;
			}
		});
}

void RehtiGraphics::Backend::createFramebuffers()
{
	size_t swapChainSize = swapChainImageViews.size();
	frameBuffers.resize(swapChainSize);

	for (size_t i = 0u; i < swapChainSize; i++)
	{
		std::array<VkImageView, 2> attachments = { swapChainImageViews[i], depthImage.view };

		VkFramebufferCreateInfo frameInfo{};
		frameInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameInfo.renderPass = renderPass;
		frameInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		frameInfo.pAttachments = attachments.data();
		frameInfo.width = swapChainExtent.width;
		frameInfo.height = swapChainExtent.height;
		frameInfo.layers = 1;
		VK_CHECK(vkCreateFramebuffer(logDevice, &frameInfo, nullptr, &frameBuffers[i]), "failed to create a framebuffer");
	}

	cleanupStack.push([this]()
		{
			for (VkFramebuffer framebuffer : frameBuffers)
			{
				if (framebuffer != VK_NULL_HANDLE)
				{
					vkDestroyFramebuffer(logDevice, framebuffer, nullptr);
				}
			}
			frameBuffers.clear();
		});
}

void RehtiGraphics::Backend::createCommandPool()
{
	auto queuefamilyIndices = findQueueFamilies(this->physDevice, this->surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queuefamilyIndices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VK_CHECK(vkCreateCommandPool(logDevice, &poolInfo, nullptr, &commandPool), "Failed to create a command pool");
	cleanupStack.push([this]()
		{
			if (commandPool != VK_NULL_HANDLE)
			{
				vkDestroyCommandPool(logDevice, commandPool, nullptr);
				commandPool = VK_NULL_HANDLE;
			}
			commandBuffers.clear();
		});
}

void RehtiGraphics::Backend::createCommandBuffers()
{
	commandBuffers.resize(frameBuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(logDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers");
}

void RehtiGraphics::Backend::createSynchronization()
{
	imageAvailableSemaphores.resize(NUM_BUFFERED_FRAMES);
	renderFinishedSemaphores.resize(NUM_BUFFERED_FRAMES);
	inFlightFences.resize(NUM_BUFFERED_FRAMES);

	VkSemaphoreCreateInfo semaInfo{};
	semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < NUM_BUFFERED_FRAMES; i++)
	{
		VK_CHECK(vkCreateSemaphore(logDevice, &semaInfo, nullptr, &imageAvailableSemaphores[i])
			| vkCreateSemaphore(logDevice, &semaInfo, nullptr, &renderFinishedSemaphores[i])
			| vkCreateFence(logDevice, &fenceInfo, nullptr, &inFlightFences[i]), "Creating synchros failed");
	}

	cleanupStack.push([this]()
		{
			for (VkFence fence : inFlightFences)
			{
				if (fence != VK_NULL_HANDLE)
				{
					vkDestroyFence(logDevice, fence, nullptr);
				}
			}
			inFlightFences.clear();

			for (VkSemaphore sem : renderFinishedSemaphores)
			{
				if (sem != VK_NULL_HANDLE)
				{
					vkDestroySemaphore(logDevice, sem, nullptr);
				}
			}
			renderFinishedSemaphores.clear();

			for (VkSemaphore sem : imageAvailableSemaphores)
			{
				if (sem != VK_NULL_HANDLE)
				{
					vkDestroySemaphore(logDevice, sem, nullptr);
				}
			}
			imageAvailableSemaphores.clear();
		});
}

void RehtiGraphics::Backend::createDrawQuery()
{
	this->drawQuery = world.query_builder<IndexedDrawable>()
			.with(flecs::pair<Pipeline, flecs::Wildcard>()).build();
}

void RehtiGraphics::Backend::recordDrawCommands()
{
	VkRenderingAttachmentInfo colorAttachment{};

	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;

	colorAttachment.imageView = swapChainImageViews[currentFrame];
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	colorAttachment.clearValue.color = GraphicsConstants::CLEAR_COLOR;

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachment.imageView = depthImage.view;
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = &depthAttachment;
	VkRect2D renderArea{};
	renderArea.offset = { 0, 0 };
	renderArea.extent = swapChainExtent;
	renderingInfo.renderArea = renderArea;

	vkCmdBeginRendering(commandBuffers[currentFrame], &renderingInfo);
	// Todo draw calls here.
	// flecs system = query and the corresponding function. E.g. 
	// So I guess here there could be a system for recording draw calls but seems a bit unhelpful?
	// because why would you record those commands only as part of a system? No, you want to run the draw loop as a part of the system.

	vkCmdEndRendering(commandBuffers[currentFrame]);
}

void RehtiGraphics::Backend::cleanup()
{
	if (logDevice != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(logDevice);
	}

	while (!cleanupStack.empty())
	{
		cleanupStack.top()();
		cleanupStack.pop();
	}

	if (window != nullptr)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();

	instanceExtensions.clear();
	deviceExtensions.clear();
	physDevice = VK_NULL_HANDLE;
	graphicsQueue = VK_NULL_HANDLE;
	presentQueue = VK_NULL_HANDLE;
}

void RehtiGraphics::Backend::initialize(const Settings& graphicsSettings)
{
	if (instance != VK_NULL_HANDLE)
	{
		throw std::runtime_error("Graphics already initialized");
	}
	
	this->vulkanVersion = VK_API_VERSION_1_3;

	if (hasFlag(graphicsSettings.flags, RehtiGraphicsUtil::Flags::WINDOWING))
	{
		SDL_Init(SDL_INIT_VIDEO); // This just forwards the call to initialize the video subsystem.
		cleanupStack.push([]()
			{
				SDL_Quit();
			});

		SDL_Vulkan_LoadLibrary(nullptr);
		cleanupStack.push([]()
			{
				SDL_Vulkan_UnloadLibrary();
			});

		SDL_WindowFlags window_flags = (SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
		window = SDL_CreateWindow(graphicsSettings.windowTitle.c_str(), 1280, 720, window_flags);
		if (window == nullptr)
		{
			SDL_Log("Could not create window: %s", SDL_GetError());
		}
		cleanupStack.push([this]()
			{
				if (window != nullptr)
				{
					SDL_DestroyWindow(window);
					window = nullptr;
				}
			});

		uint32_t extensionCount;
		const char* const* pExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
		if (pExtensions == NULL)
		{
			throw std::runtime_error("Could not fetch extensions");
		}
		instanceExtensions.resize(extensionCount);
		for (uint32_t i = 0; i < extensionCount; i++)
		{
			instanceExtensions[i] = pExtensions[i];
		}

		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	if (debug)
	{
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (hasFlag(graphicsSettings.flags, RehtiGraphicsUtil::Flags::DYNAMIC_RENDERING))
	{
		deviceExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
	}
	try {
		
		createInstance();
		setupDebugging();
		createSurface();
		createPhysicalDevice();
		createLogicalDevice();
		createGraphicsResourceManager();
		createSwapChain();
		createDepthResources();
		createImageViews();
		createRenderPass();
        createPipelineManager();
		createFramebuffers();
		createCommandPool();
		createCommandBuffers();
		createSynchronization();
	}
	catch (RehtiException& e)
	{
        Logger::error("Graphics initialization failed: ");
        Logger::error(e.what());
		cleanup();
	}

	// initializeGuiCapabilities();
}

RehtiGraphics::RehtiGraphics(flecs::world& world)
: EngineSubsystem(world), backendInstance(std::make_unique<Backend>())
{
	backendInstance->world = world;
}
RehtiGraphics::~RehtiGraphics() = default;

int RehtiGraphics::initialize(const Configuration& config)
{
	std::cout << "Initializing.." << std::endl;
	RehtiGraphics::Settings settings{};
	backendInstance->initialize(settings);
	return 0;
}

int RehtiGraphics::cleanup()
{
	backendInstance->cleanup();
    return 0;
}

bool RehtiGraphics::isInitialized() const
{
	return backendInstance->logDevice != VK_NULL_HANDLE;
}

void RehtiGraphics::drawFrame() const
{
	// Todo.
	// How is drawing configured
	// for enabled pipelines:
	// vkcmd bind pipeline
	// vkcmd bind desc
	//  for each compatible / enabled rendering target?
	// draw()
	//
}

CompiledShaderHandle RehtiGraphics::compileShader(const ShaderAsset& shader)
{
	flecs::entity shaderEntity = this->world.entity();
	CompiledShaderData compiledShaders = this->backendInstance->shaderTools->compileShader(shader);
	shaderEntity.set<CompiledShaderData>(compiledShaders);
    return CompiledShaderHandle(shaderEntity.id()); // todo think where these are stored XD
}

PipelineHandle RehtiGraphics::createGraphicsPipeline(const GraphicsPipelineConfig& pipelineConfig)
{
	flecs::entity pipelineEntity = this->world.entity();
    CompiledPipelineData pipeline = backendInstance->pipelineManager.createPipeline(_placeholder_, _placeholder_, _placeholder_);
	pipelineEntity.set<CompiledPipelineData>(CompiledPipelineData{});

	return PipelineHandle{pipelineEntity.id()};
}

GraphicsObjectHandle RehtiGraphics::createGraphicsObject(const Mesh& mesh)
{
	flecs::entity gfxEntity = this->world.entity();
	// 
    VkDeviceSize size = static_cast<VkDeviceSize>(mesh.getSize());
    std::vector<uint8_t> vertexDataBytes = Mapping::toBytes(mesh);
	assert(vertexDataBytes.size() == size);

	ResourceAllocationDetails allocDetails{};
	allocDetails.memUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	allocDetails.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	Buffer vertexBuffer = backendInstance->resourceManager->createBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, allocDetails);
    Buffer indexBuffer = backendInstance->resourceManager->createBuffer(mesh.indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, allocDetails);
	backendInstance->resourceManager->copyToBuffer(vertexBuffer, vertexDataBytes.data());
    backendInstance->resourceManager->copyToBuffer(indexBuffer, mesh.indices.data());
	IndexedDrawable drawable{};
	drawable.vertexBuffer = vertexBuffer.buffer;
	drawable.indexBuffer = indexBuffer.buffer;
    drawable.indexCount = static_cast<uint32_t>(mesh.indices.size());
	gfxEntity.set<IndexedDrawable>(drawable);
	return GraphicsObjectHandle{gfxEntity.id()};
}

bool RehtiGraphics::attachGraphicsObjectToPipeline(PipelineHandle pipelineHandle, GraphicsObjectHandle gfxObjectHandle)
{
    return false;
}
