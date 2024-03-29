#pragma once

#ifdef EG_BUILD_DLL
	#define EVERGREEN_API __declspec(dllexport)
#else
	#define EVERGREEN_API __declspec(dllimport)
#endif // EG_BUILD_DLL

#define WINDOW_EXCEPT( hr ) WindowException( __LINE__,__FILE__,hr )
#define WINDOW_LAST_EXCEPT() WindowException( __LINE__,__FILE__,GetLastError() )

#define ERROR_POPUP(text, caption) MessageBox(nullptr, text, caption, MB_OK | MB_ICONEXCLAMATION)

#define ND [[nodiscard]]

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

#ifdef EG_DX11

	#define GFX_THROW_NOINFO(hrcall) { HRESULT hr; if( FAILED( hr = (hrcall) ) ) throw DeviceResourcesExceptionDX11( __LINE__,__FILE__,hr ); }

	#if defined(_DEBUG)
		#define INFOMAN Evergreen::DxgiInfoManagerDX11& infoManager = Evergreen::DeviceResourcesDX11::GetInfoManager();

		#define GFX_EXCEPT(hr) Evergreen::DeviceResourcesExceptionDX11( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
		#define GFX_THROW_INFO(hrcall) { HRESULT hr; INFOMAN infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr); }
		// #define GFX_DEVICE_REMOVED_EXCEPT(hr) { INFOMAN DeviceRemovedExceptionDX11( __LINE__,__FILE__,(hr),infoManager.GetMessages() ) }
		#define GFX_THROW_INFO_ONLY(call) { INFOMAN infoManager.Set(); call; {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Evergreen::InfoException( __LINE__,__FILE__,v);}}}
	#else
		#define GFX_EXCEPT(hr) DeviceResourcesExceptionDX11( __LINE__,__FILE__,(hr) )
		#define GFX_THROW_INFO(hrcall) { HRESULT hr; if( FAILED( hr = (hrcall) ) ) std::terminate(); }
		// #define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedExceptionDX11( __LINE__,__FILE__,(hr) )
		#define GFX_THROW_INFO_ONLY(call) call;
	#endif

#elif EG_DX12

	#define GFX_THROW_NOINFO(hrcall) { HRESULT hr; if( FAILED( hr = (hrcall) ) ) throw DeviceResourcesExceptionDX12( __LINE__,__FILE__,hr ); }

	#if defined(_DEBUG)
		#define INFOMAN DxgiInfoManagerDX12& infoManager = DeviceResourcesDX12::GetInfoManager();

		#define GFX_EXCEPT(hr) DeviceResourcesExceptionDX12( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
		#define GFX_THROW_INFO(hrcall) { HRESULT hr; INFOMAN infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr); }
		// #define GFX_DEVICE_REMOVED_EXCEPT(hr) { INFOMAN DeviceRemovedExceptionDX11( __LINE__,__FILE__,(hr),infoManager.GetMessages() ) }
		#define GFX_THROW_INFO_ONLY(call) { INFOMAN infoManager.Set(); call; {auto v = infoManager.GetMessages(); if(!v.empty()) {throw InfoException( __LINE__,__FILE__,v);}}}
	#else
		#define GFX_EXCEPT(hr) DeviceResourcesExceptionDX12( __LINE__,__FILE__,(hr) )
		#define GFX_THROW_INFO(hrcall) { HRESULT hr; if( FAILED( hr = (hrcall) ) ) std::terminate(); }
		// #define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedExceptionDX11( __LINE__,__FILE__,(hr) )
		#define GFX_THROW_INFO_ONLY(call) call;
	#endif

#elif EG_OPENGL

#define GFX_THROW_NOINFO(hrcall)

#elif EG_VULKAN

#define GFX_THROW_NOINFO(hrcall)

#endif
