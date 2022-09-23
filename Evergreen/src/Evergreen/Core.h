#pragma once

#ifdef EG_PLATFORM_WINDOWS
	#ifdef EG_BUILD_DLL
		#define EVERGREEN_API __declspec(dllexport)
	#else
		#define EVERGREEN_API __declspec(dllimport)
	#endif // EG_BUILD_DLL

#else
	#error Evergreen only supports Windows
#endif // EG_PLATFORM_WINDOWS