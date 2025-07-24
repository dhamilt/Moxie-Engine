#include "pch.h"
#include "VkSetup.h"

namespace Spunk
{
	bool VkSetup::IsPlatformSupported()
	{
#if defined(__APPLE__)
		return VK_FALSE;
#else 
		return VK_TRUE;
#endif
	}

	bool VkSetup::CreateInstance(SDL_Window* window)
	{
		if (IsPlatformSupported())
		{
			VkResult result;
			auto vkInfo = GetInfo();
			// TODO: Create a flagging system to dynamically add all required extensions for instance
		// Retrieve the number of extensions for SDL to work with Vulkan
			if (!SDL_Vulkan_GetInstanceExtensions(window, &vkInfo->extensionCount, NULL))
			{
				perror("Error! Unable to find the required amount of Vulkan extensions!");
				return false;
			}

			// Load in the extensions
			vkInfo->extensions = std::vector<const char*>(vkInfo->extensionCount);
			if (!SDL_Vulkan_GetInstanceExtensions(window, &vkInfo->extensionCount, vkInfo->extensions.data()))
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
			vkInfo->extensions.push_back("VK_KHR_display");
			vkInfo->extensionCount++;

			// if deploying a debug build of engine
#if _DEBUG
		// Initialize Vulkan Validation Layers
			vkInfo->layers.push_back("VK_LAYER_KHRONOS_validation");
			vkInfo->layerCount++;			

			vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &supportedExtensionCount, VK_NULL_HANDLE);
			supportedExtensions.resize(supportedExtensionCount);
			vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &supportedExtensionCount, supportedExtensions.data());

			// Enable debug report extension
			vkInfo->extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			vkInfo->extensionCount++;


#endif // DEBUG

			// Setup application info
			auto _appInfo = &vkInfo->appInfo;
			_appInfo->pApplicationName = "Moxie Engine";
			_appInfo->applicationVersion = 1;
			_appInfo->pEngineName = "LunarG SDK";
			_appInfo->engineVersion = 1;
			_appInfo->apiVersion = VK_HEADER_VERSION_COMPLETE;

			// Setup instance creation info
			VkInstanceCreateInfo initInfo{};
				initInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				initInfo.pNext = VK_NULL_HANDLE,
				initInfo.flags = vkInfo->instanceFlags,
				initInfo.pApplicationInfo = _appInfo,
				initInfo.enabledExtensionCount = vkInfo->extensionCount,
				initInfo.ppEnabledExtensionNames = vkInfo->extensions.data(),
				initInfo.enabledLayerCount = vkInfo->layerCount,
				initInfo.ppEnabledLayerNames = vkInfo->layers.data(),


			// Create vulkan instance
			result = vkCreateInstance(&initInfo, vkInfo->allocationCallbacks, &vkInfo->instance);

			//// Initialize ImGui window
			//vkInfo->window = ImGui_ImplVulkanH_Window();



			assert(result == VK_SUCCESS);
			return true;
		}

		return VK_FALSE;
	}

	bool VkSetup::SetupDebugCallbacks()
	{
#if _DEBUG
	
		auto vkInfo = GetInfo();
		// Get the function pointer for extensions
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vkInfo->instance, "vkCreateDebugUtilsMessengerEXT"));
		assert(vkCreateDebugUtilsMessengerEXT != NULL);

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
		debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugUtilsMessengerCreateInfo.pNext = VK_NULL_HANDLE;
		debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debugUtilsMessengerCreateInfo.flags = 0;
		debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugUtilsMessengerCreateInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugUtilsMessengerCallback;

		VkResult result = vkCreateDebugUtilsMessengerEXT(vkInfo->instance, &debugUtilsMessengerCreateInfo, vkInfo->allocationCallbacks, &vkInfo->debugMessengerCallback);

		//Moxie::VKErrorReporting(result);
		if (result != VK_SUCCESS)
		{
			fprintf(stderr, "Unable to create debug callback! Error code:%d", result);
			return VK_FALSE;
		}
#endif
		return VK_TRUE;
	}

	VkInitInfo* VkSetup::GetInfo()
	{
		// TODO: Create implementation that supports multiple threads accessing this
		if (!infoPtr)
			infoPtr = new VkInitInfo();

		return infoPtr;
	}

	std::string GetVkObjectName(const VkObjectType& objectType)
	{
		std::string objName;
		switch (objectType)
		{
		case VK_OBJECT_TYPE_UNKNOWN:
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

	VkBool32 DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagBitsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
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


}