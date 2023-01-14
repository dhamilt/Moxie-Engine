#include "glPCH.h"
#include "VulkanPlatformInit.h"

PVulkanPlatformInit::PVulkanPlatformInit()
{
}
PVulkanPlatformInit* PVulkanPlatformInit::Get()
{
    // TODO: Create implementation that supports multiple threads accessing this
    if (instance == nullptr)
    {
        instance = new PVulkanPlatformInit();
        instance->currentVKSettings = PVulkanPlatformInitInfo();

    }
    return instance;
}

bool PVulkanPlatformInit::CreateInstance(SDL_Window* window)
{
    if (InitializePlatform())
    {
        VkResult result;
        
        

        // Retrieve the number of extensions for SDL to work
  // with Vulkan
        if (!SDL_Vulkan_GetInstanceExtensions(window, &currentVKSettings.extensionCount, NULL))
        {
            perror("Error! Unable to find the required amount of Vulkan extensions!");
            return false;
        }

        // Load in the extensions
        currentVKSettings.extensions = std::vector<const char*>(currentVKSettings.extensionCount);
        if (!SDL_Vulkan_GetInstanceExtensions(window, &currentVKSettings.extensionCount, currentVKSettings.extensions.data()))
        {
            perror("Error! Unable to load Vulkan extensions!");
            return false;
        }

        // if deploying a debug build of engine
#if _DEBUG
        // Initialize Vulkan Validation Layers
        currentVKSettings.layers.push_back("VK_LAYER_KHRONOS_validation");
        currentVKSettings.layerCount++;

        // Enable debug report extension
        currentVKSettings.extensions.push_back("VK_EXT_debug_report");
        currentVKSettings.extensionCount++;

        
#endif // DEBUG
        // Setup application info
        auto _appInfo = &currentVKSettings.appInfo;
        _appInfo->pApplicationName = "Moxie Engine";
        _appInfo->applicationVersion = 1;
        _appInfo->pEngineName = "LunarG SDK";
        _appInfo->engineVersion = 1;
        _appInfo->apiVersion = VK_API_VERSION_1_0;

        // Setup instance creation info
        auto _initInfo = &currentVKSettings.initInfo;
        _initInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        _initInfo->flags = currentVKSettings.initFlags;
        _initInfo->pApplicationInfo = _appInfo;
        _initInfo->enabledExtensionCount = currentVKSettings.extensionCount;
        _initInfo->ppEnabledExtensionNames = currentVKSettings.extensions.data();
        _initInfo->enabledLayerCount = currentVKSettings.layerCount;
        _initInfo->ppEnabledLayerNames = currentVKSettings.layers.data();

        // Create vulkan instance
        result = vkCreateInstance(&currentVKSettings.initInfo, currentVKSettings.allocationCallback, &currentVKSettings.instance);

        //// Initialize ImGui window
        //currentVKSettings.window = ImGui_ImplVulkanH_Window();

       

        //assert(result == VK_SUCCESS);
        return true;
    }
    return false;
}

bool PVulkanPlatformInit::ImGuiVkSetup(SDL_Window* window)
{
    VkResult result;
  
#if _DEBUG
   
    // Get the function pointer for extensions
    auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(currentVKSettings.instance, "vkCreateDebugReportCallbackEXT");
    assert(vkCreateDebugReportCallbackEXT != NULL);

    // Setup debug report callback
    VkDebugReportCallbackCreateInfoEXT debugReportExt_cb = {};
    debugReportExt_cb.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugReportExt_cb.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debugReportExt_cb.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugReportCallback;
    debugReportExt_cb.pUserData = NULL;
    

    result = vkCreateDebugReportCallbackEXT(currentVKSettings.instance, &debugReportExt_cb, currentVKSettings.allocationCallback, &currentVKSettings.debugReportCallback);

    Moxie::VKErrorReporting(result);
    if (result != VK_SUCCESS)
    {
        perror("Error! Unabl to create Debug Report Callback!");
        return false;
    }
#endif

    // Select which GPU to use
    result = vkEnumeratePhysicalDevices(currentVKSettings.instance, &currentVKSettings.physicalDeviceCount, NULL);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to retrieve the number of GPUs on machine!");
        return false;
    }
   
    currentVKSettings.physicalDevices = std::vector<VkPhysicalDevice>(currentVKSettings.physicalDeviceCount);
    result = vkEnumeratePhysicalDevices(currentVKSettings.instance, &currentVKSettings.physicalDeviceCount, currentVKSettings.physicalDevices.data());

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to retrieve GPU info!");
        return false;
    }

    // If machine is using a multi-gpu setup, use the discrete gpu
    // otherwise use the first gpu
    int index = 0;
    for (auto it = currentVKSettings.physicalDevices.begin(); it != currentVKSettings.physicalDevices.end(); it++)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(*it, &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            currentVKSettings.discreteGPUIndex = index;
            break;
        }
        index++;
    }

    // Select graphics queue family
    auto device = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &currentVKSettings.queueCount, NULL);
    currentVKSettings.queueFamilyProperties = std::vector<VkQueueFamilyProperties>(currentVKSettings.queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &currentVKSettings.queueCount, currentVKSettings.queueFamilyProperties.data());
    index = 0;
    for (auto it = currentVKSettings.queueFamilyProperties.begin(); it != currentVKSettings.queueFamilyProperties.end(); it++)
    {
        if (it->queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            currentVKSettings.queueFamily = index;
            break;
        }
        index++;
    }

    if (currentVKSettings.queueFamily == (VkBool32)-1)
    {
        perror("Error! Unable to retrieve the graphics queue family!");
        return false;
    }


    // Create a Logical Device using 1 queue
    // TODO: Decouple this process into it's
    // own separate functionality


    auto devExtCount = &currentVKSettings.deviceExtCount;
    auto devExtensions = &currentVKSettings.deviceExtensions;
    devExtensions->push_back("VK_KHR_swapchain");
    auto queueInfo = &currentVKSettings.queueInfo;
    auto deviceInfo = &currentVKSettings.deviceInfo;

    auto _queue = VkDeviceQueueCreateInfo();
    _queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    _queue.queueFamilyIndex = currentVKSettings.queueFamily;
    _queue.queueCount = 1;
    _queue.pQueuePriorities = &currentVKSettings.queuePriority;
    queueInfo->push_back(_queue);

    deviceInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo->queueCreateInfoCount = (uint32_t)queueInfo->size();
    deviceInfo->pQueueCreateInfos = queueInfo->data();
    deviceInfo->enabledExtensionCount = *devExtCount;
    deviceInfo->ppEnabledExtensionNames = devExtensions->data();

    result = vkCreateDevice(device, deviceInfo, currentVKSettings.allocationCallback, &currentVKSettings.device);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create device!");
        return false;
    }

    vkGetDeviceQueue(currentVKSettings.device, currentVKSettings.queueFamily, 0, &currentVKSettings.queue);

    // Create a Descriptor Pool
    auto poolSizes = &currentVKSettings.poolSizes;
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 });
    poolSizes->push_back({ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 });

    auto poolInfo = &currentVKSettings.poolInfo;
    poolInfo->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo->flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo->maxSets = 1000 * (uint32_t)poolSizes->size();
    poolInfo->poolSizeCount = (uint32_t)poolSizes->size();
    poolInfo->pPoolSizes = poolSizes->data();
    
    result = vkCreateDescriptorPool(currentVKSettings.device, poolInfo, currentVKSettings.allocationCallback, &currentVKSettings.descriptorPool);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create the descriptor pool!");
        return false;
    }


    // Setup ImGui/Vulkan backends
    auto imInitInfo = &currentVKSettings.imGuiInitInfo;
    imInitInfo->Allocator = currentVKSettings.allocationCallback;
    imInitInfo->DescriptorPool = currentVKSettings.descriptorPool;
    imInitInfo->Instance = currentVKSettings.instance;
    imInitInfo->Device = currentVKSettings.device;
    imInitInfo->PhysicalDevice = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
    imInitInfo->MinImageCount = currentVKSettings.minImageCount;
    imInitInfo->CheckVkResultFn = Moxie::VKErrorReporting;

    return true;
}

//bool PVulkanPlatformInit::SetupVulkanWindow(VkSurfaceKHR surface, int width, int height)
//{
//    auto window = &currentVKSettings.window;
//    auto physDevice = &currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
//    window->Surface = surface;
//
//    // Check for WSI support
//    VkBool32 result;
//    vkGetPhysicalDeviceSurfaceSupportKHR(*physDevice, currentVKSettings.queueFamily, window->Surface, &result);
//
//    if (result != VK_TRUE)
//    {
//        perror("Error! No WSI support on this physical device!");
//        return false;
//    }
//
//    // Select Surface format
//    auto requestSurfaceFormats = &currentVKSettings.surfaceFormats;
//    requestSurfaceFormats->push_back(VK_FORMAT_B8G8R8A8_UNORM);
//    requestSurfaceFormats->push_back(VK_FORMAT_R8G8B8A8_UNORM);
//    requestSurfaceFormats->push_back(VK_FORMAT_R8G8B8_UNORM);
//
//    auto requestSurfaceColorSpaces = &currentVKSettings.surfaceColorSpaces;
//    requestSurfaceColorSpaces->push_back(VK_COLORSPACE_SRGB_NONLINEAR_KHR);
//    window->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(*physDevice, window->Surface, requestSurfaceFormats->data(),
//        (int)requestSurfaceFormats->size(), requestSurfaceColorSpaces->front());
//    auto framerateCheck = currentVKSettings.limitFramerate;
//    auto presentModes = &currentVKSettings.presentModes;
//    if (framerateCheck)
//    {
//        presentModes->push_back(VK_PRESENT_MODE_MAILBOX_KHR);
//        presentModes->push_back(VK_PRESENT_MODE_IMMEDIATE_KHR);
//    }
//
//    presentModes->push_back(VK_PRESENT_MODE_FIFO_KHR);
//    window->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(*physDevice, window->Surface,
//                            presentModes->data(), (int)presentModes->size());
//
//    assert(currentVKSettings.minImageCount >= 2);
//    ImGui_ImplVulkanH_CreateOrResizeWindow(currentVKSettings.instance, *physDevice, currentVKSettings.device, window, currentVKSettings.queueFamily,
//                        currentVKSettings.allocationCallback, width, height, currentVKSettings.minImageCount);
//    return false;
//}

bool PVulkanPlatformInit::IsSupported()
{
#if defined(__APPLE__)
    return false;
#else
    return true;
#endif

}

bool PVulkanPlatformInit::InitializePlatform()
{
    if (!IsSupported())
    {
        using std::exception;
        using std::string;
        throw std::runtime_error(string("Unable to initialize to Vulkan! \nThe current operating system is not supported!"));
        return false;
    }
    else
    {

        return true;
    }
}

// Returns all available layers that the Vulkan instance can use on this machine
void PVulkanPlatformInit::GetInstanceLayers(VkBool32& layerCount, std::vector<VkLayerProperties>& properties)
{
    if (currentVKSettings.instance != NULL)
    {
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        properties = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, &properties[0]);
    }
}

// Returns all available layers on the default Vulkan Physical Device can use on this machine
void PVulkanPlatformInit::GetDeviceLayers(VkBool32& layerCount, std::vector<VkLayerProperties>& properties)
{
    if (currentVKSettings.discreteGPUIndex != (VkBool32)-1)
    {
        auto device = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
        vkEnumerateDeviceLayerProperties(device, &layerCount, nullptr);
        properties = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateDeviceLayerProperties(device, &layerCount, &properties[0]);
    }
}

// Retrieves all of the available extensions for the Vulkan instance
void PVulkanPlatformInit::GetInstanceExtensions(VkBool32& extCount, std::vector<VkExtensionProperties>& properties)
{
    if (currentVKSettings.instance != NULL)
    {
        vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
        properties = std::vector<VkExtensionProperties>(extCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &properties[0]);
    }
}
// Retrieves all of the available extensions on the discrete GPU being used
void PVulkanPlatformInit::GetDeviceExtensions(VkBool32& extCount, std::vector<VkExtensionProperties>& properties)
{
    if (currentVKSettings.discreteGPUIndex != (VkBool32)-1)
    {
        auto device = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
        properties = std::vector<VkExtensionProperties>(extCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, &properties[0]);
    }
}

void PVulkanPlatformInit::CleanupVulkan()
{
    // Wait for device to be in idle state
    vkDeviceWaitIdle(currentVKSettings.device);
    vkDestroyDescriptorPool(currentVKSettings.device, currentVKSettings.descriptorPool, currentVKSettings.allocationCallback);

#if defined(_DEBUG)
    // Remove debug report callback
    auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(currentVKSettings.instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT(currentVKSettings.instance, currentVKSettings.debugReportCallback, currentVKSettings.allocationCallback);
#endif
    vkDestroyDevice(currentVKSettings.device, currentVKSettings.allocationCallback);
    vkDestroyInstance(currentVKSettings.instance, currentVKSettings.allocationCallback);
}



//void PVulkanPlatformInit::CleanupVulkanWindow()
//{
//    ImGui_ImplVulkanH_DestroyWindow(currentVKSettings.instance, currentVKSettings.device, &currentVKSettings.window, currentVKSettings.allocationCallback);
//}

PVulkanPlatformInitInfo* PVulkanPlatformInit::GetInfo()
{
    return &currentVKSettings;
}

