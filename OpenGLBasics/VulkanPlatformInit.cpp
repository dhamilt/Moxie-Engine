#include "glPCH.h"
#include "VulkanPlatformInit.h"

PVulkanPlatformInit::PVulkanPlatformInit()
{
}

// Create a descriptor pool for adding descriptors for buffers being passed into shaders
bool PVulkanPlatformInit::CreateDescriptorPool()
{
    VkResult result;

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

    return true;
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

bool PVulkanPlatformInit::CreateFence(VkFence** fencePtr)
{
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // Create the fence with the Create Signaled flag,
    //so the fence can wait before using it on a GPU command (for the first frame)
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(currentVKSettings.device, &fenceInfo, currentVKSettings.allocationCallback, &currentVKSettings.inFlightFence);
    *fencePtr = &currentVKSettings.inFlightFence;
    return true;
}

void PVulkanPlatformInit::GetWindowExtent(VkExtent2D& windowExtent)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    assert(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex], currentVKSettings.surface, &surfaceCapabilities) == VK_SUCCESS);
    windowExtent = surfaceCapabilities.currentExtent;
}

bool PVulkanPlatformInit::CreateInstance(SDL_Window* window)
{
    if (InitializePlatform())
    {
        VkResult result;
        
        
        // TODO: Create a flagging system to dynamically add all required extensions for instance
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

        VkBool32 supportedExtensionCount;
        std::vector<VkExtensionProperties> supportedExtensions;
        vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &supportedExtensionCount, VK_NULL_HANDLE);
        supportedExtensions = std::vector<VkExtensionProperties>(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &supportedExtensionCount, &supportedExtensions[0]);

        // Add Display mode extension
        currentVKSettings.extensions.push_back("VK_KHR_display");
        currentVKSettings.extensionCount++;

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
        _appInfo->apiVersion = VK_HEADER_VERSION_COMPLETE;

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

       

        assert(result == VK_SUCCESS);
        return true;
    }
    return false;
}

// Retrieve the discrete gpu 
bool PVulkanPlatformInit::GetPhysicalDevices()
{
    // Select which GPU to use
    VkResult result = vkEnumeratePhysicalDevices(currentVKSettings.instance, &currentVKSettings.physicalDeviceCount, NULL);

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
    vkGetPhysicalDeviceQueueFamilyProperties(device, &currentVKSettings.queueFamilyCount, NULL);
    currentVKSettings.queueFamilyProperties = std::vector<VkQueueFamilyProperties>(currentVKSettings.queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &currentVKSettings.queueFamilyCount, currentVKSettings.queueFamilyProperties.data());
    index = 0;
    for (auto it = currentVKSettings.queueFamilyProperties.begin(); it != currentVKSettings.queueFamilyProperties.end(); it++)
    {
        if (it->queueFlags & VK_QUEUE_GRAPHICS_BIT)
            currentVKSettings.queueFamilies.push_back(index);
        index++;
    }

    if (currentVKSettings.queueFamilies.size() < 1)
    {
        perror("Error! Unable to retrieve the graphics queue family!");
        return false;
    }

    return true;
}

bool PVulkanPlatformInit::SetupDebugCallbacks()
{
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


    VkResult result = vkCreateDebugReportCallbackEXT(currentVKSettings.instance, &debugReportExt_cb, currentVKSettings.allocationCallback, &currentVKSettings.debugReportCallback);

    Moxie::VKErrorReporting(result);
    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create Debug Report Callback!");
        return false;
    }
#endif

    return true;
}

bool PVulkanPlatformInit::ImGuiVkSetup(SDL_Window* window)
{

    // Setup ImGui/Vulkan backends
    auto imInitInfo = &currentVKSettings.imGuiInitInfo;
    imInitInfo->Allocator = currentVKSettings.allocationCallback;
    imInitInfo->DescriptorPool = currentVKSettings.descriptorPool;
    imInitInfo->Instance = currentVKSettings.instance;
    imInitInfo->Device = currentVKSettings.device;
    imInitInfo->PhysicalDevice = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
    imInitInfo->MinImageCount = currentVKSettings.minImageCount;
    imInitInfo->CheckVkResultFn = Moxie::VKErrorReporting;
    imInitInfo->MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    imInitInfo->Queue = currentVKSettings.queue;
    imInitInfo->QueueFamily = currentVKSettings.queueFamilies[0];
    imInitInfo->ImageCount = (VkBool32)currentVKSettings.imageBuffers.size();
    imInitInfo->PipelineCache = currentVKSettings.pipelineCache;
    imInitInfo->Subpass = 0;


    return true;
}

bool PVulkanPlatformInit::IsSupported()
{
#if defined(__APPLE__)
    return false;
#else
    return true;
#endif

}
/// <summary>
/// Creates two sync objects (semaphores) 
/// One for syncing to the render loop
/// the other for syncing to the image being presented from the previous frame
/// </summary>
/// <param name="presentSemaphorePtr"></param>
/// <param name="renderSemaphore"></param>
/// <returns></returns>
bool PVulkanPlatformInit::CreateSemaphores(VkSemaphore** presentSemaphorePtr, VkSemaphore** renderSemaphorePtr)
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.flags = 0;
    semaphoreInfo.pNext = VK_NULL_HANDLE;
    VkResult result = vkCreateSemaphore(currentVKSettings.device, &semaphoreInfo, currentVKSettings.allocationCallback, &currentVKSettings.imageAvailableSemaphore);
    assert(result == VK_SUCCESS);
    result = vkCreateSemaphore(currentVKSettings.device, &semaphoreInfo, currentVKSettings.allocationCallback, &currentVKSettings.renderFinishedSemaphore);
    assert(result == VK_SUCCESS);
    *presentSemaphorePtr = &currentVKSettings.imageAvailableSemaphore;
    *renderSemaphorePtr = &currentVKSettings.renderFinishedSemaphore;
    return true;
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

VkPresentModeKHR PVulkanPlatformInit::SetPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
{
    for (const auto& mode : availableModes)
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return VK_PRESENT_MODE_MAILBOX_KHR;


    return VK_PRESENT_MODE_FIFO_KHR;
}

void PVulkanPlatformInit::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkFormat imgFormat)
{
  
}

// Create a Logical Device using 1 queue
bool PVulkanPlatformInit::CreateLogicalDeviceAndQueue()
{
    currentVKSettings.deviceExtensions.push_back("VK_KHR_swapchain");
    currentVKSettings.deviceExtensions.push_back("VK_EXT_depth_range_unrestricted");
    auto queueInfo = &currentVKSettings.queueInfo;
    auto deviceInfo = &currentVKSettings.deviceInfo;

    auto _queue = VkDeviceQueueCreateInfo();
    _queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    _queue.queueFamilyIndex = currentVKSettings.queueFamilies[0];
    _queue.queueCount = 1;
    _queue.pQueuePriorities = &currentVKSettings.queuePriority;
    queueInfo->push_back(_queue);

    deviceInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo->queueCreateInfoCount = (uint32_t)queueInfo->size();
    deviceInfo->pQueueCreateInfos = queueInfo->data();
    deviceInfo->enabledExtensionCount = (VkBool32)currentVKSettings.deviceExtensions.size();
    deviceInfo->ppEnabledExtensionNames = currentVKSettings.deviceExtensions.data();
    auto device = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
    VkResult result = vkCreateDevice(device, deviceInfo, currentVKSettings.allocationCallback, &currentVKSettings.device);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create device!");
        return false;
    }

    vkGetDeviceQueue(currentVKSettings.device, currentVKSettings.queueFamilies[0], 0, &currentVKSettings.queue);
    return true;
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

bool PVulkanPlatformInit::CreateCommandPool(VkCommandBuffer** commandBuffer)
{
 
    // Create a command pool for command buffer actions to be placed
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex   = currentVKSettings.queueFamilies[0];
    poolInfo.flags              = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.pNext              = NULL;

    auto device = currentVKSettings.device;
    auto result =  vkCreateCommandPool(device, &poolInfo, currentVKSettings.allocationCallback, &currentVKSettings.commandPool);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create command pool!");
        return false;
    }

    VkCommandBufferAllocateInfo bufferInfo = {};
    bufferInfo.sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferInfo.commandPool          = currentVKSettings.commandPool;
    bufferInfo.commandBufferCount   = MAX_COMMAND_POOL_SIZE;
    bufferInfo.level                = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    // TODO: allocate for only command buffers that have been requested
    currentVKSettings.commandBuffers = std::vector<VkCommandBuffer>(MAX_COMMAND_POOL_SIZE);
    result = vkAllocateCommandBuffers(device, &bufferInfo, &currentVKSettings.commandBuffers[0]);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to allocate for command buffer(s)!");
        return false;
    }
    *commandBuffer = &currentVKSettings.commandBuffers[0];
    return true;

}

bool PVulkanPlatformInit::CreateSwapChain()
{
    VkBool32 surfaceFormatCount;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    GetSupportedImageFormats(surfaceFormatCount, surfaceFormats);
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    auto physicalDevice = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
    auto surfaceKHR = currentVKSettings.surface;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surfaceKHR, &surfaceCapabilities);
    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to pull surface capabilities from physical device!");
        return false;
    }
    
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surfaceKHR, &surfaceFormatCount, VK_NULL_HANDLE);
    
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surfaceKHR, &surfaceFormatCount, &surfaceFormats[0]);


    // Create swapchain for managing and switching between image buffers on vulkan surface
    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = currentVKSettings.surface;
    swapchainInfo.pNext = NULL;
    swapchainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surfaceKHR, &currentVKSettings.presentModeCount, NULL);
    currentVKSettings.presentModes = std::vector<VkPresentModeKHR>(currentVKSettings.presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surfaceKHR, &currentVKSettings.presentModeCount, &currentVKSettings.presentModes[0]);
    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to pull surface present modes from physical device!");
        return false;
    }

    // Make one more than the swapchain requires
    // as long as it is permitted
    swapchainInfo.minImageCount = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageCount,
        surfaceCapabilities.maxImageCount,surfaceCapabilities.minImageCount+1);
    currentVKSettings.minImageCount = swapchainInfo.minImageCount;
    // Clamp swapchain image extent to surface extent threshold
    swapchainInfo.imageExtent.height = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height, surfaceCapabilities.currentExtent.height);
    swapchainInfo.imageExtent.width = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.currentExtent.width);
    
    // Set swapchain's starting transform to use default transform if surface supports it
    // otherwise use the current surface transform
    swapchainInfo.preTransform = surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ?
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform;

    // Find a supported composite alpha mode - one of these is guaranteed to be set
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (auto it = compositeAlphaFlags.begin(); it != compositeAlphaFlags.end(); it++) {
        if (surfaceCapabilities.supportedCompositeAlpha & *it) {
            swapchainInfo.compositeAlpha = *it;
            break;
        }
    }
    
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.presentMode = SetPresentMode(currentVKSettings.presentModes);
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 1;
    swapchainInfo.pQueueFamilyIndices = &currentVKSettings.queueFamilies[0];
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo.clipped = true;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    auto device = currentVKSettings.device;
    // Create the swapchain
    result = vkCreateSwapchainKHR(device, &swapchainInfo, currentVKSettings.allocationCallback, &currentVKSettings.swapchain);
    
    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create swapchain!");
        return false;
    }
    
    // Retrieve the swapchain images
    vkGetSwapchainImagesKHR(device, currentVKSettings.swapchain, &currentVKSettings.swapchainImageCount, NULL);
    currentVKSettings.swapchainImages = std::vector<VkImage>(currentVKSettings.swapchainImageCount);
    result = vkGetSwapchainImagesKHR(device, currentVKSettings.swapchain, 
        &currentVKSettings.swapchainImageCount, &currentVKSettings.swapchainImages[0]);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to retrieve images from swapchain!");
        return false;
    }

    //currentVKSettings.imageBuffers = std::vector<PVkImageBuffer>(currentVKSettings.swapchainImageCount);
    // Create image view(s) for the swapchain
    for (VkBool32 i = 0; i < currentVKSettings.swapchainImageCount; i++)
    {
        PVkImageBuffer img = {};
        img.image = currentVKSettings.swapchainImages[i];
        VkImageViewCreateInfo imgViewInfo = {};
        imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgViewInfo.pNext = nullptr;
        imgViewInfo.flags = 0;
        imgViewInfo.image = img.image;
        imgViewInfo.format = swapchainInfo.imageFormat;
        imgViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        imgViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        imgViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        imgViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgViewInfo.subresourceRange.baseMipLevel = 0;
        imgViewInfo.subresourceRange.levelCount = 1;
        imgViewInfo.subresourceRange.baseArrayLayer = 0;
        imgViewInfo.subresourceRange.layerCount = 1;
        imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        result = vkCreateImageView(device, &imgViewInfo, currentVKSettings.allocationCallback, &img.imageView);
        assert(result == VK_SUCCESS);
        currentVKSettings.imageBuffers.push_back(img);
    };
    currentVKSettings.swapchainImages.clear();

    // Create Depth Buffer
    VkImageCreateInfo imgCreateInfo = {};
    imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgCreateInfo.pNext = VK_NULL_HANDLE;
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
    imgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imgCreateInfo.extent.width = swapchainInfo.imageExtent.width;
    imgCreateInfo.extent.height = swapchainInfo.imageExtent.height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.samples = VK_NUM_OF_SAMPLES;
    imgCreateInfo.queueFamilyIndexCount = 1;
    imgCreateInfo.pQueueFamilyIndices = &currentVKSettings.queueFamilies[0];
    imgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgCreateInfo.flags = 0;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    result = vkCreateImage(device, &imgCreateInfo, currentVKSettings.allocationCallback, &currentVKSettings.depthBuffer.image);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create depth buffer image object!");
        return false;
    }

    // Allocate memory for the depth buffer
    VkMemoryRequirements depthBufMemReqs;
    vkGetImageMemoryRequirements(device, currentVKSettings.depthBuffer.image, &depthBufMemReqs);
    VkMemoryAllocateInfo depthBufMemoryInfo = {};
    depthBufMemoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    depthBufMemoryInfo.allocationSize = depthBufMemReqs.size;

    VkDeviceMemory depthBufMemory;
    result = vkAllocateMemory(device, &depthBufMemoryInfo, currentVKSettings.allocationCallback, &depthBufMemory);
    
    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to allocate memory for the Depth Buffer!");
        return false;
    }
    // Bind memory buffer to depth buffer
    vkBindImageMemory(device, currentVKSettings.depthBuffer.image, depthBufMemory, 0);

    auto depthViewInfo = &currentVKSettings.depthViewInfo;
    depthViewInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthViewInfo->pNext = VK_NULL_HANDLE;
    depthViewInfo->flags = 0;
    depthViewInfo->image = currentVKSettings.depthBuffer.image;
    depthViewInfo->format = VK_FORMAT_D24_UNORM_S8_UINT;
    depthViewInfo->components.r = VK_COMPONENT_SWIZZLE_R;
    depthViewInfo->components.g = VK_COMPONENT_SWIZZLE_G;
    depthViewInfo->components.b = VK_COMPONENT_SWIZZLE_B;
    depthViewInfo->components.a = VK_COMPONENT_SWIZZLE_A;
    depthViewInfo->subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthViewInfo->subresourceRange.baseMipLevel = 0;
    depthViewInfo->subresourceRange.levelCount = 1;
    depthViewInfo->subresourceRange.baseArrayLayer = 0;
    depthViewInfo->subresourceRange.layerCount = 1;
    depthViewInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;

    result = vkCreateImageView(device, depthViewInfo, currentVKSettings.allocationCallback, &currentVKSettings.depthBuffer.imageView);

    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to create depth buffer image view!");
        return false;
    }
    return true;
}

void PVulkanPlatformInit::CleanupVulkan()
{
    // Wait for device to be in idle state
    auto device = currentVKSettings.device;
    vkDeviceWaitIdle(device);
    vkDestroyFence(device, currentVKSettings.inFlightFence, currentVKSettings.allocationCallback);
    vkDestroySemaphore(device, currentVKSettings.imageAvailableSemaphore, currentVKSettings.allocationCallback);
    vkDestroySemaphore(device, currentVKSettings.renderFinishedSemaphore, currentVKSettings.allocationCallback);
    vkFreeCommandBuffers(device, currentVKSettings.commandPool, MAX_COMMAND_POOL_SIZE, &currentVKSettings.commandBuffers[0]);
    vkDestroyRenderPass(device, currentVKSettings.renderPass, currentVKSettings.allocationCallback);
    vkDestroyCommandPool(device, currentVKSettings.commandPool, currentVKSettings.allocationCallback);
    for (VkBool32 i = 0; i < currentVKSettings.swapchainImageCount; i++)
        vkDestroyImageView(device, currentVKSettings.imageBuffers[i].imageView, currentVKSettings.allocationCallback);
    vkDestroySwapchainKHR(device, currentVKSettings.swapchain, currentVKSettings.allocationCallback);

    vkDestroyDescriptorPool(device, currentVKSettings.descriptorPool, currentVKSettings.allocationCallback);

#if defined(_DEBUG)
    // Remove debug report callback
    auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(currentVKSettings.instance, "vkDestroyDebugReportCallbackEXT");
    vkDestroyDebugReportCallbackEXT(currentVKSettings.instance, currentVKSettings.debugReportCallback, currentVKSettings.allocationCallback);
#endif
    vkDestroyDevice(device, currentVKSettings.allocationCallback);
    vkDestroyInstance(currentVKSettings.instance, currentVKSettings.allocationCallback);

}

void PVulkanPlatformInit::GetSupportedImageFormats(VkBool32& formatCount, std::vector<VkSurfaceFormatKHR>& supportedFormats)
{
    auto physicalDevice = currentVKSettings.physicalDevices[currentVKSettings.discreteGPUIndex];
    auto surface = currentVKSettings.surface;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, VK_NULL_HANDLE);
    assert(result == VK_SUCCESS);
    supportedFormats = std::vector<VkSurfaceFormatKHR>(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, &supportedFormats[0]);
    assert(result == VK_SUCCESS);
}

// TODO: implement functionality to retrieve the highest level
// of graphical fidelity supported on current gpu


PVulkanPlatformInitInfo* PVulkanPlatformInit::GetInfo()
{
    return &currentVKSettings;
}

bool PVulkanPlatformInit::CreateRenderPass()
{
    // Create color attachment(s)
    VkAttachmentDescription colorAttachmentInfo = {};
    colorAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
    colorAttachmentInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Create depth attachment
    VkAttachmentDescription depthAttachmentInfo = {};
    depthAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
    depthAttachmentInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    std::vector<VkAttachmentDescription> attachments(2);
    attachments[0] = colorAttachmentInfo;
    attachments[1] = depthAttachmentInfo;
    

    VkAttachmentReference colorRef = {};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  

    VkAttachmentReference depthRef = {};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Create Subpass info
    VkSubpassDescription subpassDescription = {};
    subpassDescription.flags = 0;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorRef;
    subpassDescription.pDepthStencilAttachment = &depthRef;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = VK_NULL_HANDLE;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics subpass
    subpassDescription.pResolveAttachments = VK_NULL_HANDLE;
    subpassDescription.pInputAttachments = VK_NULL_HANDLE;
   

    // Create subpass dependency for auto transititoning between image layouts
    VkSubpassDependency dependency = {};
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pNext = VK_NULL_HANDLE;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
  
    renderPassInfo.pAttachments = &attachments[0];
    renderPassInfo.flags = NULL;
    VkResult result = vkCreateRenderPass(currentVKSettings.device, &renderPassInfo, currentVKSettings.allocationCallback, &currentVKSettings.renderPass);
    assert(result == VK_SUCCESS);
    return true;
}

