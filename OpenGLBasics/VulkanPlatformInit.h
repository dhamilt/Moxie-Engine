#pragma once
#include "PlatformInit.h"
#include <imgui_impl_vulkan.h>
#include <SDL_vulkan.h>

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
    VkBool32 queueFamily = 0;
    VkBool32 queueCount = 0;
    float queuePriority = 1.0f;
    VkQueue  queue;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
   

    VkDebugReportCallbackEXT debugReportCallback;
    VkPipelineCache pipelineCache;

    VkDescriptorPool descriptorPool;
    VkDescriptorPoolCreateInfo poolInfo;
    std::vector<VkDescriptorPoolSize> poolSizes;

    VkInstanceCreateFlags initFlags;
    VkInstanceCreateInfo initInfo;
    VkApplicationInfo appInfo;
    VkSurfaceKHR surface;
    std::vector<VkFormat> surfaceFormats;
    std::vector<VkColorSpaceKHR> surfaceColorSpaces;

    std::vector<VkPresentModeKHR> presentModes;
    bool limitFramerate = false;

    ImGui_ImplVulkanH_Window window;
    ImGui_ImplVulkan_InitInfo imGuiInitInfo;    
};

static VkBool32 DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
    size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, const char* pUserData);
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
    //bool SetupVulkanWindow(VkSurfaceKHR surface, int width, int height);
    void CleanupVulkan();
    //void CleanupVulkanWindow();
    PVulkanPlatformInitInfo* GetInfo();
    static PVulkanPlatformInit* Get();
    void operator=(const PVulkanPlatformInit& other) = delete;
protected:
    virtual bool IsSupported() override;
    virtual bool InitializePlatform() override;
    PVulkanPlatformInit();

private:
    PVulkanPlatformInit(const PVulkanPlatformInit& other);
    PVulkanPlatformInitInfo currentVKSettings;

};
static PVulkanPlatformInit* instance;

VkBool32 DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
    size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, const char* pUserData)
{
    /*Ignored arguments*/
    (void)flags; 
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    (void)pLayerPrefix;

    fprintf(stderr, "Vulkan Debug Report from ObjectType: %i \nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}