#pragma once

#ifdef SPUNKFRAMEWORK_EXPORTS
#define SPUNKFRAMEWORK_API __declspec(dllexport)
#else
#define SPUNKFRAMEWORK_API __declspec(dllimport)
#endif // SPUNKFRAMEWORK_EXPORTS


namespace Spunk {

	struct VkInitInfo 
	{
		VkAllocationCallbacks* allocationCallbacks;
		VkInstance instance = NULL;
		VkInstanceCreateFlags instanceFlags;
		VkApplicationInfo appInfo;
		VkBool32 extensionCount = 0;
		VkBool32 layerCount = 0;
		std::vector<const char*> extensions;
		std::vector<const char*> layers;


		VkDebugUtilsMessengerEXT debugMessengerCallback;
	};

	static VkBool32 DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagBitsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	static std::string GetVkObjectName(const VkObjectType& objectType);

	class SPUNKFRAMEWORK_API VkSetup
	{
	public:
		bool IsPlatformSupported();
		bool CreateInstance(SDL_Window* window);
		bool SetupDebugCallbacks();
		static VkInitInfo* GetInfo();

	};

	static VkInitInfo* infoPtr;

}

