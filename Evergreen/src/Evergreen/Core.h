#pragma once

#ifdef EG_PLATFORM_WINDOWS
	#ifdef EG_BUILD_DLL
		#define EVERGREEN_API __declspec(dllexport)
	#else
		#define EVERGREEN_API __declspec(dllimport)
	#endif // EG_BUILD_DLL

	#define WINDOW_EXCEPT( hr ) WindowsWindowException( __LINE__,__FILE__,hr )
	#define WINDOW_LAST_EXCEPT() WindowsWindowException( __LINE__,__FILE__,GetLastError() )

	#define ERROR_POPUP(text, caption) MessageBox(nullptr, text, caption, MB_OK | MB_ICONEXCLAMATION)

#else
	#error Evergreen only supports Windows
#endif // EG_PLATFORM_WINDOWS



#ifdef EG_ENABLE_ASSERTS
	#define EG_ASSERT(x, ...) { if (!(x)) { EG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define EG_CORE_ASSERT(x, ...) { if (!(x)) { EG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define EG_ASSERT(x, ...)
	#define EG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define TERMINATE_ON_THROW(fn)	\
	try {																								\
		fn;																								\
	}																									\
    catch (const BaseException& e)																		\
	{																									\
		ERROR_POPUP(e.what(), e.GetType());																\
		std::terminate();																				\
	}																									\
	catch (const std::exception& e)																		\
	{																									\
		ERROR_POPUP(e.what(), "Standard Exception");													\
		std::terminate();																				\
	}																									\
	catch (...)																							\
	{																									\
		ERROR_POPUP("No details available", "Unknown Exception");										\
		std::terminate();																				\
	}

