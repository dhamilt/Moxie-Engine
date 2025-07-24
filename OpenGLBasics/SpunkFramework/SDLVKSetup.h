#pragma once

#ifdef SPUNKFRAMEWORK_EXPORTS
#define SPUNKFRAMEWORK_API __declspec(dllexport)
#else
#define SPUNKFRAMEWORK_API __declspec(dllimport)
#endif // SPUNKFRAMEWORK_EXPORTS

namespace Spunk
{
	class SPUNKFRAMEWORK_API SDLVKSetup
	{
	public:
		static  SDLVKSetup* Get();
		// Creates an SDL Window as well as initializes a Vulkan rendering pipeline
		bool InitializeProject();
	private:
		SDLVKSetup() {};
	};

	static SDLVKSetup* instance;
}

