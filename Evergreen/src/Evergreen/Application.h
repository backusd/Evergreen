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
};
#pragma warning( pop )

// To be defined in CLIENT
Application* CreateApplication();

}