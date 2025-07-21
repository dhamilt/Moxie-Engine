#pragma once
#include "PlatformInit.h"
#include <imgui_impl_vulkan.h>
#include <SDL_vulkan.h>
#include <vector>
#include <string>


#define VK_NUM_OF_SAMPLES VK_SAMPLE_COUNT_1_BIT
struct PVkImageBuffer
{
    VkImage image;
    VkImageView imageView;
};

struct PVulkanPlatformInitInfo
{    
    VkInstance instance = NULL;
    VkBool32 extensionCount = 0;
    VkBool32 layerCount = 0;
    std::vector<const char*> extensions;
    std::vector<const char*> layers;
    VkAllocationCallbacks* allocationCallback;
    VkDevice device;
    std::vector<VkPhysicalDevice> physicalDevices;
    VkBool32 discreteGPUIndex = (VkBool32)-1;
    VkBool32 physicalDeviceCount = 0;
    VkBool32 deviceExtCount = 0;
    std::vector<const char*> deviceExtensions;
    std::vector<VkDeviceQueueCreateInfo> queueInfo;
    VkDeviceCreateInfo deviceInfo;
    
    VkBool32 minImageCount = 0;
    std::vector<VkBool32> queueFamilies;
    VkBool32 queueFamilyCount = 0;
    float queuePriority = 1.0f;
    VkQueue  queue;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
   
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkDebugReportCallbackEXT debugReportCallback;
    VkPipelineCache pipelineCache;

    VkDescriptorPool descriptorPool;
    VkDescriptorPoolCreateInfo poolInfo;
    std::vector<VkDescriptorPoolSize> poolSizes;

    VkDescriptorSetAllocateInfo descriptorInfo;
    std::vector<VkDescriptorSet> descriptorSets;

    VkInstanceCreateFlags initFlags;
    VkInstanceCreateInfo initInfo;
    VkApplicationInfo appInfo;
    VkSurfaceKHR surface;
    VkFormat surfaceFormat;
    std::vector<VkColorSpaceKHR> surfaceColorSpaces;

    std::vector<VkPresentModeKHR> presentModes;
    VkBool32 presentModeCount;
    bool limitFramerate = false;

	VkPhysicalDeviceSynchronization2Features synchroFeatures;

	VkImageViewCreateInfo swapchainImgViewInfo;
	VkSwapchainCreateInfoKHR swapchainInfo;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    PVkImageBuffer depthBuffer;
    VkImageViewCreateInfo depthViewInfo;
    VkBool32 swapchainImageCount;
    std::vector<PVkImageBuffer> swapChainImgBufs;

    VkRenderPass renderPass;
    std::vector<VkFence> inFlightFences;
    std::vector<VkSemaphore> renderFinishedSemaphores, imageAvailableSemaphores;

    ImGui_ImplVulkanH_Window window;
    ImGui_ImplVulkan_InitInfo imGuiInitInfo;

    VkDebugUtilsMessengerEXT debugMessengerCallback;
};

static VkBool32 const MAX_COMMAND_POOL_SIZE = 32;
static VkBool32 const MAX_COMMAND_BUFFER_SIZE = 1024;

static VkBool32 DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
	size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, const char* pUserData);

static VkBool32 DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagBitsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

static void AllocationCallback(void* pUserdata, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);

static std::string GetVkObjectName(const VkObjectType& objectType);

class PVulkanPlatformInit :
    public PPlatformInit
{
public:
    bool CreateInstance(SDL_Window* window);
    bool ImGuiVkSetup(SDL_Window* window);
    void GetInstanceLayers(VkBool32& layerCount, std::vector<VkLayerProperties>& properties);
    void GetDeviceLayers(VkBool32& layerCount, std::vector<VkLayerProperties>& properties);
    void GetInstanceExtensions(VkBool32& extCount, std::vector<VkExtensionProperties>& properties);
    void GetDeviceExtensions(VkBool32& extCount, std::vector<VkExtensionProperties>& properties);
    void GetSupportedImageFormats(VkBool32& formatCount, std::vector<VkSurfaceFormatKHR>& supportedFormats);
    bool CreateCommandPool(VkCommandBuffer* commandBuffers);
    void GetWindowExtent(VkExtent2D& windowExtent);
    bool GetPhysicalDevices();
    bool SetupDebugCallbacks();
    bool SetupAllocationCallbacks();
    bool CreateLogicalDeviceAndQueue();
    bool CreateDescriptorPool();
    bool CreateSwapChain();
    bool CreateRenderPass();
    bool CreateSemaphores(VkSemaphore* presentSemaphorePtr, VkSemaphore* renderSemaphorePtr);
    bool CreateFences(VkFence* fencePtr);
    //bool SetupVulkanWindow(VkSurfaceKHR surface, int width, int height);
    void CleanupVulkan();
	// Choose the Mailbox Present Mode ("Triple Buffering")
   // if available, otherwise default to FIFO Mode
	VkPresentModeKHR SetPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
    PVulkanPlatformInitInfo* GetInfo();
    static PVulkanPlatformInit* Get();
    void operator=(const PVulkanPlatformInit& other) = delete;
protected:
    virtual bool IsSupported() override;
    virtual bool InitializePlatform() override;
    
    void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkFormat imgFormat);
    PVulkanPlatformInit();

private:
    PVulkanPlatformInit(const PVulkanPlatformInit& other);
    PVulkanPlatformInitInfo currentVKSettings;

};
static PVulkanPlatformInit* instance;

std::string GetVkObjectName(const VkObjectType& objectType)
{
    std::string objName;
    switch (objectType)
    {
    case VK_OBJECT_TYPE_UNKNOWN :
        objName = "Unknown Object";
        break;
	case VK_OBJECT_TYPE_INSTANCE:
		objName = "Vulkan instance";
		break;
	case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
		objName = "Physical Device";
		break;
	case VK_OBJECT_TYPE_DEVICE:
		objName = "Logical Device";
		break;
	case VK_OBJECT_TYPE_QUEUE:
		objName = "Device Queue";
		break;
	case VK_OBJECT_TYPE_SEMAPHORE:
		objName = "Semaphore Sync Object";
		break;
	case VK_OBJECT_TYPE_COMMAND_BUFFER:
		objName = "Command Buffer";
		break;
	case VK_OBJECT_TYPE_FENCE:
		objName = "Fence Sync";
		break;
	case VK_OBJECT_TYPE_DEVICE_MEMORY:
		objName = "Device Memory";
		break;
	case VK_OBJECT_TYPE_BUFFER:
		objName = "Buffer Handle";
		break;
	case VK_OBJECT_TYPE_IMAGE:
		objName = "Image Handle";
		break;
	case VK_OBJECT_TYPE_EVENT:
		objName = "Event Handle";
		break;
	case VK_OBJECT_TYPE_QUERY_POOL:
		objName = "Query Pool Handle";
		break;
	case VK_OBJECT_TYPE_BUFFER_VIEW:
		objName = "Buffer View Handle";
		break;
	case VK_OBJECT_TYPE_IMAGE_VIEW:
		objName = "Image View Handle";
		break;
	case VK_OBJECT_TYPE_SHADER_MODULE:
		objName = "Shader Module";
		break;
	case VK_OBJECT_TYPE_PIPELINE_CACHE:
		objName = "Pipeline Cache";
		break;
	case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
		objName = "Pipeline Layout Handle";
		break;
	case VK_OBJECT_TYPE_RENDER_PASS:
		objName = "Renderpass Handle";
		break;
	case VK_OBJECT_TYPE_PIPELINE:
		objName = "Pipeline Handle";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
		objName = "Descriptor Set Layout Handle";
		break;
	case VK_OBJECT_TYPE_SAMPLER:
		objName = "Sampler Handle";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
		objName = "Descriptor Pool Handle";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_SET:
		objName = "Descriptor Set Handle";
		break;
	case VK_OBJECT_TYPE_FRAMEBUFFER:
		objName = "Framebuffer Handle";
		break;
	case VK_OBJECT_TYPE_COMMAND_POOL:
		objName = "Command Pool Handle";
		break;
	case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
		objName = "Sampler YCBCR Conversion Handle";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
		objName = "Descriptor Update Template Handle";
		break;
	case VK_OBJECT_TYPE_PRIVATE_DATA_SLOT:
		objName = "Private Data Slot Handle";
		break;
	case VK_OBJECT_TYPE_SURFACE_KHR:
		objName = "KHR Surface Handle";
		break;
	case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
		objName = "KHR Swapchain Handle";
		break;
	case VK_OBJECT_TYPE_DISPLAY_KHR:
		objName = "KHR Display Handle";
		break;
	case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
		objName = "Display Mode";
		break;
	case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
		objName = "Debug Report Callback Handle";
		break;
	case VK_OBJECT_TYPE_VIDEO_SESSION_KHR:
		objName = "KHR Video Session";
		break;
	case VK_OBJECT_TYPE_VIDEO_SESSION_PARAMETERS_KHR:
		objName = "KHR Video Session Parameters";
		break;
	case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
		objName = "Debug Utils Messenger Callback Handle";
		break;
	case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR:
		objName = "KHR Acceleration Handle";
		break;
	case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
		objName = "Invalidation Cache Handle";
		break;
	case VK_OBJECT_TYPE_SHADER_EXT:
		objName = "Shader Extension Handle";
		break;
	case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV:
		objName = "Raytracing Handle";
		break;
	case VK_OBJECT_TYPE_PIPELINE_BINARY_KHR:
		objName = "KHR Pipeline Binary";
		break;
	case VK_OBJECT_TYPE_MICROMAP_EXT:
		objName = "Opacity Micromap Extension Handle";
		break;
    default:
        objName = "Unknown Object";
        break;
    }
    return objName;
}
VkBool32 DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
    size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, const char * pUserData)
{
    /*Ignored arguments*/
    (void)flags; 
    (void)object;
    (void)pUserData;
    (void)pLayerPrefix;

    fprintf(stderr, "Vulkan Debug Report from ObjectType: %i \nMessage: %s\nLocation: %lld\nCode: %i\n\n", objectType, pMessage, location, messageCode);
    return VK_FALSE;
}

VkBool32 DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagBitsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	// if no discernable data provided from callback
	if (!(severity ^ VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT) && !(messageTypes ^ VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT))
		return VK_FALSE;

	// if only general info provided
	if (!(severity ^ VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) && !(messageTypes ^ VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT))
		return VK_FALSE;

	std::string output;
	if (severity ^ VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT)
	{
		output = "Severity: ";

		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			output += "[ERROR] ";		
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			output += "[WARNING] ";
		if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			output += "[VERBOSE] ";
	}
	if ((messageTypes ^ VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT) && (messageTypes ^ VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT))
	{
		output += "\nType: ";

		if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			output += "[VALIDATION] ";
		if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			output += "[PERFORMANCE] ";
		if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
			output += "[DEVICE ADDRESS BINDING] ";

		output += "\nId Name:";
		output += pCallbackData->pMessageIdName;
		output += "\nId#";
		output += std::to_string(pCallbackData->messageIdNumber);
		output += "\nMessage: ";
		output += pCallbackData->pMessage;
		if (pCallbackData->objectCount)
		{
			std::string plural = pCallbackData->objectCount > 1 ? " objects, which are:\n" : " object, which is:\n";
			output += "\nIssue involves " + std::to_string(pCallbackData->objectCount) + plural;
			if (pCallbackData->objectCount > 0)
			{
				for (uint32_t i = 0; i < pCallbackData->objectCount; ++i)
				{
					output += "\t- " + GetVkObjectName(pCallbackData->pObjects[i].objectType) + " ";
					if (pCallbackData->pObjects[i].pObjectName)
						output += pCallbackData->pObjects[i].pObjectName;
					output += "\n";
				}
			}
		}
		output += "\n";
	}
	
	printf(output.c_str());

	return VK_TRUE;
}


void AllocationCallback(void* pUserdata, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    /**Ignored arguments*/
    (void)pUserdata;

    std::string scope;
    switch (allocationScope)
    {
    case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND:
        scope = "command buffer";
        break;

	case VK_SYSTEM_ALLOCATION_SCOPE_OBJECT:
		scope = "an object";
		break;

	case VK_SYSTEM_ALLOCATION_SCOPE_CACHE:
		scope = "pipeline cache or validation cache";
		break;

	case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE:
		scope = "logical device";
		break;

	case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE:
		scope = "vulkan instance";
		break;
    }
    std::string output = "Allocation size: " + std::to_string(size) + "\nAlignment: " + std::to_string(alignment) + "\nScoped to: " + scope + ".";
    fprintf(stderr, output.c_str());
}