#pragma once
#include "pch.h"
#include "Core.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"
#include "UI/Layout.h"
#include "UI/UI.h"
#include "Evergreen/Window/Window.h"
#include "Rendering/DeviceResources.h"
#include "Evergreen/Utils/Timer.h"

// See: https://learn.microsoft.com/en-us/cpp/c-runtime-library/debug-versions-of-heap-allocation-functions?view=msvc-170
#if defined(DEBUG) || defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Application
{
public:
	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;
	virtual ~Application() {};

	int Run();

protected:
	std::unique_ptr<UI> m_ui;
	std::shared_ptr<DeviceResources> m_deviceResources;
	Timer m_timer;
	std::shared_ptr<Window> m_window;

	// Virtual functions so the client application can update & render to viewports 
	virtual void OnUpdate(const Timer& timer) {}
	virtual void OnRender() {}

private:
	void Update(const Timer& timer);
	void Render();
	void Present();

	void OnWindowResize(WindowResizeEvent& e);
	void OnWindowCreate(WindowCreateEvent& e);
	void OnWindowClose(WindowCloseEvent& e);
	void OnAppTick(AppTickEvent& e);
	void OnAppUpdate(AppUpdateEvent& e);
	void OnAppRender(AppRenderEvent& e);
	void OnChar(CharEvent& e);
	void OnKeyPressed(KeyPressedEvent& e);
	void OnKeyReleased(KeyReleasedEvent& e);
	void OnMouseMove(MouseMoveEvent& e);
	void OnMouseEnter(MouseEnterEvent& e);
	void OnMouseLeave(MouseLeaveEvent& e);
	void OnMouseScrolledVertical(MouseScrolledEvent& e);
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e);
	void OnMouseButtonPressed(MouseButtonPressedEvent& e);
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e);



// There is a somewhat weird behavior in DirectX reporting memory leaks on application shutdown.
// In a DEBUG build, DirectX will report on any DirectX resources that have outstanding reference
// counts. However, this check appears to be performed prior to static class instance destruction.
// This means that if we have a static class (ex. any singleton class) with a reference to DirectX 
// resources (such as a shared_ptr to DeviceResources), DirectX will report a memory leak because
// the static instance of the class has not yet been destructed.
// 
// In this specific case, JSONLoaders keeps a cache of all Styles that have been loaded and each
// style has a shared_ptr to DeviceResources. So here, we just clean that up, which is also just a
// good idea because there is no need to keep the cached styles around anyways.
	class ClearCache
	{
	public:
		~ClearCache() 
		{ 
			Evergreen::JSONLoaders::ClearCache(); 
		}
	};

	ClearCache m_cc;
};
#pragma warning( pop )

// To be defined in CLIENT
Application* CreateApplication();

}