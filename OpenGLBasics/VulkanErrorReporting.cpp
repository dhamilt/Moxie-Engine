#include "glPCH.h"
#include "VulkanErrorReporting.h"

void Moxie::VKErrorReporting(VkResult result)
{
	switch (result)
	{
	case VK_SUCCESS:
		printf("No error found.");
		break;

	case VK_NOT_READY:
		perror("Vulkan is not ready!");
		break;

	case VK_TIMEOUT:
		perror("Vulkan has timed out!");
		break;

	case VK_INCOMPLETE:
		perror("Vulkan setup is incomplete!");
		break;

	case VK_ERROR_OUT_OF_HOST_MEMORY:
		perror("Vulkan host is out of memory!");
		break;

	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		perror("Vulkan device is out of memory!");
		break;

	case VK_ERROR_INITIALIZATION_FAILED:
		perror("Vulkan was not initialized properly!");
		break;

	case VK_ERROR_DEVICE_LOST:
		perror("Vulkan device was lost!");
		break;

	case VK_ERROR_MEMORY_MAP_FAILED:
		perror("Vulkan memory mapping has failed!");
		break;

	case VK_ERROR_LAYER_NOT_PRESENT:
		perror("Vulkan layer is being used, but is not loaded!");
		break;

	case VK_ERROR_EXTENSION_NOT_PRESENT:
		perror("Vulkan extension is being used, but is not loaded!");
		break;

	case VK_ERROR_FEATURE_NOT_PRESENT:
		perror("Vulkan feature is being used, but is not loaded!");
		break;

	case VK_ERROR_INCOMPATIBLE_DRIVER:
		perror("Vulkan is using an incompatible driver!");
		break;

	case VK_ERROR_TOO_MANY_OBJECTS:
		perror("There are too many objects being used in Vulkan!");
		break;

	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		perror("Vulkan format not supported!");
		break;

	case VK_ERROR_FRAGMENTED_POOL:
		perror("Vulkan memory pool is fragmented!");
		break;

	case VK_ERROR_UNKNOWN:
		perror("Unknown Vulkan error has occured!");
		break;

	case VK_ERROR_OUT_OF_POOL_MEMORY:
		perror("Vulkan pool is out of memory!");
		break;

	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		perror("An external handle being used by Vulkan is invalid!");
		break;

	case VK_ERROR_FRAGMENTATION:
		perror("A fragmentation error has occured!");
		break;

	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
		perror("Invalid capture address!");
		break;

	case VK_PIPELINE_COMPILE_REQUIRED:
		perror("Vulkan pipeline requires a compile!");
		break;

	case VK_ERROR_SURFACE_LOST_KHR:
		perror("Vulkan surface is missing a KHR!");
		break;

	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		perror("Vulkan Native Window is currently is use!");
		break;

	case VK_SUBOPTIMAL_KHR:
		perror("Vulkan KHR is suboptimal!");
		break;

	case VK_ERROR_OUT_OF_DATE_KHR:
		perror("Vulkan KHR is out of date!");
		break;

	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		perror("Vulkan KHR is incompatible!");
		break;

	case VK_ERROR_VALIDATION_FAILED_EXT:
		perror("Vulkan extension has failed validation!");
		break;

	case VK_ERROR_INVALID_SHADER_NV:
		perror("Invalid Vulkan Shader!");
		break;

	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
		perror("Invalid DRM format!");
		break;

	case VK_ERROR_NOT_PERMITTED_KHR:
		perror("Vulkan KHR is not allowed!");
		break;

	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		perror("Vulkan exclusive fullscreen mode extension is lost!");
		break;

	case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
		perror("Vulkan compression has been exhausted!");
		break;

	default:
		printf("No error found.");
		break;

	}
}
