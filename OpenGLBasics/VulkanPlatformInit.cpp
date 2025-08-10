#include "glPCH.h"
#include "VulkanPlatformInit.h"
#include "VulkanFunctionLibrary.h"
#include "GLSetup.h"

#if __APPLE__
bool isSupported = false;
#else
bool isSupported = true;
#endif



PVulkanPlatformInit* PVulkanPlatformInit::Get()
{
	assert(isSupported);
	// TODO: Create implementation that supports multiple threads accessing this
	if (instance == nullptr)
	{
		instance = new PVulkanPlatformInit();
		instance->currentVKSettings = PVulkanPlatformInitInfo();
	}
	return instance;
}

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


bool PVulkanPlatformInit::CreateFences()
{   
    for(int i = 0; i < currentVKSettings.frameData.size(); ++i)
    {
        auto frameData = &currentVKSettings.frameData[i];
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // Create the fence with the Create Signaled flag,
        //so the fence can wait before using it on a GPU command (for the first frame)
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkResult result = vkCreateFence(currentVKSettings.device, &fenceInfo, currentVKSettings.allocationCallback, &frameData->Fence);
        if (result != VK_SUCCESS)
        {
            perror("Unable to create in flight fence!");
            return false;
        }
    }
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
    //auto framework = new Spunk::VkSetup();
    //if (framework->IsPlatformSupported())
    //    return framework->CreateInstance(window);
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
 

        vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &supportedExtensionCount, VK_NULL_HANDLE);
        supportedExtensions.resize(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &supportedExtensionCount, supportedExtensions.data());

        // Enable debug report extension
        currentVKSettings.extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>( vkGetInstanceProcAddr(currentVKSettings.instance, "vkCreateDebugUtilsMessengerEXT"));
    assert(vkCreateDebugUtilsMessengerEXT != NULL);

     VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.pNext = VK_NULL_HANDLE;
    debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT|
                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    debugUtilsMessengerCreateInfo.flags = 0;
    debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT 
                                                | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT 
                                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
                                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugUtilsMessengerCallback;

    VkResult result = vkCreateDebugUtilsMessengerEXT(currentVKSettings.instance, &debugUtilsMessengerCreateInfo, currentVKSettings.allocationCallback, &currentVKSettings.debugMessengerCallback);

    Moxie::VKErrorReporting(result);
    if (result != VK_SUCCESS)
    {
        fprintf(stderr, "Unable to create debug callback! Error code:%d", result);
        return VK_FALSE;
    }
#endif

    return VK_TRUE;
}

bool PVulkanPlatformInit::SetupAllocationCallbacks()
{
    /*currentVKSettings.allocationCallback = new VkAllocationCallbacks();
    currentVKSettings.allocationCallback->pfnAllocation = (PFN_vkAllocationFunction)AllocationCallback;*/

    return VK_TRUE;
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
    imInitInfo->ImageCount = (VkBool32)currentVKSettings.swapChainImgBufs.size();
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
bool PVulkanPlatformInit::CreateSemaphores(VkSemaphore* presentSemaphorePtr, VkSemaphore* renderSemaphorePtr)
{
    currentVKSettings.imageAvailableSemaphores.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
    currentVKSettings.renderFinishedSemaphores.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);
    /*VkSemaphoreTypeCreateInfo semaphoreTypeInfo;
    semaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphoreTypeInfo.pNext = VK_NULL_HANDLE;
    semaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    semaphoreTypeInfo.initialValue = 0;*/
	VkSemaphoreCreateInfo semaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0
	};
    for (VkBool32 i = 0; i < MAX_VULKAN_FRAMES_IN_FLIGHT; i++)
    {
        
        VkResult result = vkCreateSemaphore(currentVKSettings.device, &semaphoreInfo, currentVKSettings.allocationCallback, &currentVKSettings.imageAvailableSemaphores[i]);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Unable to create Image Available Semaphore #" + std::to_string(i) + "!");
        }
        result = vkCreateSemaphore(currentVKSettings.device, &semaphoreInfo, currentVKSettings.allocationCallback, &currentVKSettings.renderFinishedSemaphores[i]);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Unable to create Rendering Finished Semaphore #" + std::to_string(i) + "!");
        }
    }
    memcpy(presentSemaphorePtr,  currentVKSettings.imageAvailableSemaphores.data(), sizeof(currentVKSettings.imageAvailableSemaphores[0]) * MAX_VULKAN_FRAMES_IN_FLIGHT);
    memcpy(renderSemaphorePtr, currentVKSettings.renderFinishedSemaphores.data(), sizeof(currentVKSettings.renderFinishedSemaphores[0]) * MAX_VULKAN_FRAMES_IN_FLIGHT);

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
    deviceInfo->pNext = VK_NULL_HANDLE;
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

bool PVulkanPlatformInit::CreateCommandPools()
{
    currentVKSettings.frameData = std::vector<ImGui_ImplVulkanH_Frame>(MAX_VULKAN_FRAMES_IN_FLIGHT);
    // Create a command pool for command buffer actions to be placed
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex   = currentVKSettings.queueFamilies[0];
    poolInfo.flags              = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.pNext              = NULL;

    auto device = currentVKSettings.device;
    for(int i = 0; i < MAX_VULKAN_FRAMES_IN_FLIGHT; ++i)
    {
		auto result = vkCreateCommandPool(device, &poolInfo, currentVKSettings.allocationCallback, &currentVKSettings.frameData[i].CommandPool);

		if (result != VK_SUCCESS)
		{
			perror("Error! Unable to create command pool!");
			return false;
		}

		VkCommandBufferAllocateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferInfo.commandPool = currentVKSettings.frameData[i].CommandPool;
		bufferInfo.commandBufferCount = 1;
		bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		// Allocate command buffer for each frame in flight		
		result = vkAllocateCommandBuffers(device, &bufferInfo, &currentVKSettings.frameData[i].CommandBuffer);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to allocate for command buffer(s)!");
			return false;
		}
    }

    
   
    return true;

}

bool PVulkanPlatformInit::CreateOneOffCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = currentVKSettings.queueFamilies[0];
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.pNext = NULL;

	auto result = vkCreateCommandPool(currentVKSettings.device, &poolInfo, currentVKSettings.allocationCallback, &currentVKSettings.oneOffCommandPool);

	if (result != VK_SUCCESS)
	{
		perror("Error! Unable to create command pool!");
		return false;
	}
    return true;
}

ImGui_ImplVulkanH_Frame* PVulkanPlatformInit::GetCurrentFrameData(VkBool32 frameIndex)
{
  return &currentVKSettings.frameData[frameIndex];        
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
    
    // Make sure that surface supports the copying of swapchain images
    assert(surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surfaceKHR, &surfaceFormatCount, VK_NULL_HANDLE);
    
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surfaceKHR, &surfaceFormatCount, &surfaceFormats[0]);


    // Create swapchain for managing and switching between image buffers on vulkan surface
    auto swapchainInfo = &currentVKSettings.swapchainInfo;
    swapchainInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo->surface = currentVKSettings.surface;
    swapchainInfo->pNext = NULL;
    swapchainInfo->imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    currentVKSettings.surfaceFormat = swapchainInfo->imageFormat;

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
    swapchainInfo->minImageCount = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageCount,
        surfaceCapabilities.maxImageCount,surfaceCapabilities.minImageCount+1);
    currentVKSettings.minImageCount = swapchainInfo->minImageCount;
    // Clamp swapchain image extent to surface extent threshold
    swapchainInfo->imageExtent.height = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height, surfaceCapabilities.currentExtent.height);
    swapchainInfo->imageExtent.width = MathLibrary<uint32_t>::Clamp(surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.currentExtent.width);
    
    // Set swapchain's starting transform to use default transform if surface supports it
    // otherwise use the current surface transform
    swapchainInfo->preTransform = surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ?
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfaceCapabilities.currentTransform;

    // Find a supported composite alpha mode - one of these is guaranteed to be set
    swapchainInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (auto it = compositeAlphaFlags.begin(); it != compositeAlphaFlags.end(); it++) {
        if (surfaceCapabilities.supportedCompositeAlpha & *it) {
            swapchainInfo->compositeAlpha = *it;
            break;
        }
    }
    
    swapchainInfo->imageArrayLayers = 1;
    swapchainInfo->presentMode = SetPresentMode(currentVKSettings.presentModes);
    swapchainInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo->queueFamilyIndexCount = 1;
    swapchainInfo->pQueueFamilyIndices = &currentVKSettings.queueFamilies[0];
    //swapchainInfo->oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo->imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo->clipped = true;
    swapchainInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    
    auto device = currentVKSettings.device;
    // Create the swapchain
    result = vkCreateSwapchainKHR(device, swapchainInfo, currentVKSettings.allocationCallback, &currentVKSettings.swapchain);
    
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

    VkImageViewCreateInfo* swapChainImgViewInfo = &currentVKSettings.swapchainImgViewInfo;
	swapChainImgViewInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	swapChainImgViewInfo->pNext = nullptr;
	swapChainImgViewInfo->flags = 0;	
	swapChainImgViewInfo->format = swapchainInfo->imageFormat;
	swapChainImgViewInfo->components.r = VK_COMPONENT_SWIZZLE_R;
	swapChainImgViewInfo->components.g = VK_COMPONENT_SWIZZLE_G;
	swapChainImgViewInfo->components.b = VK_COMPONENT_SWIZZLE_B;
	swapChainImgViewInfo->components.a = VK_COMPONENT_SWIZZLE_A;
	swapChainImgViewInfo->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	swapChainImgViewInfo->subresourceRange.baseMipLevel = 0;
	swapChainImgViewInfo->subresourceRange.levelCount = 1;
	swapChainImgViewInfo->subresourceRange.baseArrayLayer = 0;
	swapChainImgViewInfo->subresourceRange.layerCount = 1;
	swapChainImgViewInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
    // Create image view(s) for the swapchain
    for (VkBool32 i = 0; i < currentVKSettings.swapchainImageCount; i++)
    {
        PVkImageBuffer img = {};
        img.image = currentVKSettings.swapchainImages[i];        
        swapChainImgViewInfo->image = img.image;
        result = vkCreateImageView(device, swapChainImgViewInfo, currentVKSettings.allocationCallback, &img.imageView);
        assert(result == VK_SUCCESS);
        currentVKSettings.swapChainImgBufs.push_back(img);
    };
    // Create Depth Buffer
    
    VkImageCreateInfo* imgCreateInfo = &currentVKSettings.depthImageInfo;
    imgCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgCreateInfo->pNext = VK_NULL_HANDLE;
    imgCreateInfo->imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo->format = VK_FORMAT_D24_UNORM_S8_UINT;
    imgCreateInfo->usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    imgCreateInfo->extent.width = swapchainInfo->imageExtent.width;
    imgCreateInfo->extent.height = swapchainInfo->imageExtent.height;
    imgCreateInfo->extent.depth = 1;
    imgCreateInfo->mipLevels = 1;
    imgCreateInfo->arrayLayers = 1;
    imgCreateInfo->samples = VK_NUM_OF_SAMPLES;
    imgCreateInfo->queueFamilyIndexCount = 1;
    imgCreateInfo->pQueueFamilyIndices = &currentVKSettings.queueFamilies[0];
    imgCreateInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgCreateInfo->flags = 0;
    imgCreateInfo->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    result = vkCreateImage(device, imgCreateInfo, currentVKSettings.allocationCallback, &currentVKSettings.depthBuffer.image);

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

    result = vkAllocateMemory(device, &depthBufMemoryInfo, currentVKSettings.allocationCallback, &currentVKSettings.depthBuffer.memory);
    
    if (result != VK_SUCCESS)
    {
        perror("Error! Unable to allocate memory for the Depth Buffer!");
        return false;
    }
    // Bind memory buffer to depth buffer
    vkBindImageMemory(device, currentVKSettings.depthBuffer.image, currentVKSettings.depthBuffer.memory, 0);

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

void PVulkanPlatformInit::ResizeSwapChain(int _width, int _height)
{
    VkResult deviceIdle =vkDeviceWaitIdle(currentVKSettings.device);
    assert(deviceIdle == VK_SUCCESS);

    auto info =  &currentVKSettings.swapchainInfo;
    info->imageExtent =VkExtent2D(_width, _height);

    // Destroy old swapchain img views
    for (VkBool32 i = 0; i < currentVKSettings.swapChainImgBufs.size(); ++i)
    {
        VkImageView imgView = currentVKSettings.swapChainImgBufs[i].imageView;
        vkDestroyImageView(currentVKSettings.device, imgView, currentVKSettings.allocationCallback);
    }
   
    vkDestroySwapchainKHR(currentVKSettings.device, currentVKSettings.swapchain, currentVKSettings.allocationCallback);
    currentVKSettings.swapChainImgBufs.clear();
    //Destroy depth buffer image and view
    vkFreeMemory(currentVKSettings.device, currentVKSettings.depthBuffer.memory, currentVKSettings.allocationCallback);
	vkDestroyImage(currentVKSettings.device, currentVKSettings.depthBuffer.image, currentVKSettings.allocationCallback);
	vkDestroyImageView(currentVKSettings.device, currentVKSettings.depthBuffer.imageView, currentVKSettings.allocationCallback);

   
    CreateSwapChain();
}

void PVulkanPlatformInit::CleanupVulkan()
{
    // Wait for device to be in idle state
    auto device = currentVKSettings.device;
    vkDeviceWaitIdle(device);
    for (VkBool32 j = 0; j < MAX_VULKAN_FRAMES_IN_FLIGHT; j++)
    {
        vkDestroySemaphore(device, currentVKSettings.imageAvailableSemaphores[j], currentVKSettings.allocationCallback);
        vkDestroySemaphore(device, currentVKSettings.renderFinishedSemaphores[j], currentVKSettings.allocationCallback);
    }
    
    for (VkBool32 i = 0; i < currentVKSettings.frameData.size(); ++i)
    {
         ImGui_ImplVulkanH_Frame* frame = &currentVKSettings.frameData[i];
         vkFreeCommandBuffers(device, frame->CommandPool, 1, &frame->CommandBuffer);
         vkDestroyCommandPool(device, frame->CommandPool, currentVKSettings.allocationCallback);
         vkDestroyFence(device, frame->Fence, currentVKSettings.allocationCallback);         
    }
    currentVKSettings.frameData.clear();

    vkDestroyRenderPass(device, currentVKSettings.renderPass, currentVKSettings.allocationCallback);
    vkDestroyCommandPool(device, currentVKSettings.oneOffCommandPool, currentVKSettings.allocationCallback);
    for (VkBool32 i = 0; i < currentVKSettings.swapchainImageCount; i++)
        vkDestroyImageView(device, currentVKSettings.swapChainImgBufs[i].imageView, currentVKSettings.allocationCallback);
    vkDestroySwapchainKHR(device, currentVKSettings.swapchain, currentVKSettings.allocationCallback);

	//Destroy depth buffer image and view
	vkFreeMemory(currentVKSettings.device, currentVKSettings.depthBuffer.memory, currentVKSettings.allocationCallback);
	vkDestroyImage(currentVKSettings.device, currentVKSettings.depthBuffer.image, currentVKSettings.allocationCallback);
	vkDestroyImageView(currentVKSettings.device, currentVKSettings.depthBuffer.imageView, currentVKSettings.allocationCallback);

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

bool PVulkanPlatformInit::CreateImGuiFrameData(ImGui_ImplVulkanH_Frame* frames)
{
    std::vector<ImGui_ImplVulkanH_Frame> _frames;
    _frames.resize(MAX_VULKAN_FRAMES_IN_FLIGHT);

    for (VkBool32 i = 0; i < MAX_VULKAN_FRAMES_IN_FLIGHT; ++i)
    {
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = currentVKSettings.queueFamilies[0];
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.pNext = NULL;

		auto result = vkCreateCommandPool(currentVKSettings.device, &poolInfo, currentVKSettings.allocationCallback, &_frames[i].CommandPool);

		if (result != VK_SUCCESS)
		{
			perror("Error! Unable to create command pool!");
			return false;
		}

		VkCommandBufferAllocateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferInfo.commandPool = _frames[i].CommandPool;
		bufferInfo.commandBufferCount = 1;
		bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		// Allocate command buffer for each frame in flight		
		result = vkAllocateCommandBuffers(currentVKSettings.device, &bufferInfo, &_frames[i].CommandBuffer);

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to allocate for command buffer(s)!");
			return false;
		}

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		// Create the fence with the Create Signaled flag,
		//so the fence can wait before using it on a GPU command (for the first frame)
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		result = vkCreateFence(currentVKSettings.device, &fenceInfo, currentVKSettings.allocationCallback, &_frames[i].Fence);
		if (result != VK_SUCCESS)
		{
			perror("Unable to create in flight fence!");
			return false;
		}
    }
    memcpy(frames, _frames.data(), _frames.size() * sizeof(ImGui_ImplVulkanH_Frame));
    return true;
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
    colorAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    // Create depth attachment
    VkAttachmentDescription depthAttachmentInfo = {};
    depthAttachmentInfo.samples = VK_NUM_OF_SAMPLES;
    depthAttachmentInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentInfo.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    std::vector<VkAttachmentDescription> attachments(2);
    attachments[0] = colorAttachmentInfo;
    attachments[1] = depthAttachmentInfo;
    

    VkAttachmentReference colorRef = {};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  

    VkAttachmentReference depthRef = {};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkBool32 preserveAttachments[2] = {0, 1};
	// Create Color to Depth/Stencil subpass
	VkSubpassDescription colorToDepthStencilSubpass;
	colorToDepthStencilSubpass.flags = 0;
	colorToDepthStencilSubpass.colorAttachmentCount = 1;
	colorToDepthStencilSubpass.pColorAttachments = &colorRef;
	colorToDepthStencilSubpass.pDepthStencilAttachment = &depthRef;
	colorToDepthStencilSubpass.inputAttachmentCount = 0;
	colorToDepthStencilSubpass.pPreserveAttachments = VK_NULL_HANDLE;
	colorToDepthStencilSubpass.preserveAttachmentCount = 0;
	colorToDepthStencilSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Graphics subpass
	colorToDepthStencilSubpass.pResolveAttachments = VK_NULL_HANDLE;
	colorToDepthStencilSubpass.pInputAttachments = VK_NULL_HANDLE;

   
	// Create Color to Depth/Stencil attachment dependency
	VkSubpassDependency colorToDepthStencilDependency;
	colorToDepthStencilDependency.srcSubpass = 0;
	colorToDepthStencilDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
	colorToDepthStencilDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	colorToDepthStencilDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	colorToDepthStencilDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	colorToDepthStencilDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	colorToDepthStencilDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = &attachments[0];
    renderPassInfo.pNext = VK_NULL_HANDLE;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &colorToDepthStencilSubpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &colorToDepthStencilDependency;   
    renderPassInfo.flags = NULL;
    VkResult result = vkCreateRenderPass(currentVKSettings.device, &renderPassInfo, currentVKSettings.allocationCallback, &currentVKSettings.renderPass);
    assert(result == VK_SUCCESS);
    return true;
}

