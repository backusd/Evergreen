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
	Application() noexcept;
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;
	virtual ~Application() noexcept {};

	int Run() noexcept;

protected:
	std::unique_ptr<UI> m_ui;
	std::shared_ptr<DeviceResources> m_deviceResources;
	Timer m_timer;

	// Virtual functions so the client application can update & render to viewports 
	virtual void OnUpdate(const Timer& timer) {}
	virtual void OnRender() {}

private:
	void Update(const Timer& timer) noexcept;
	void Render() noexcept;
	void Present() noexcept;

	void OnWindowResize(WindowResizeEvent& e) noexcept;
	void OnWindowCreate(WindowCreateEvent& e) noexcept;
	void OnWindowClose(WindowCloseEvent& e) noexcept;
	void OnAppTick(AppTickEvent& e) noexcept;
	void OnAppUpdate(AppUpdateEvent& e) noexcept;
	void OnAppRender(AppRenderEvent& e) noexcept;
	void OnChar(CharEvent& e) noexcept;
	void OnKeyPressed(KeyPressedEvent& e) noexcept;
	void OnKeyReleased(KeyReleasedEvent& e) noexcept;
	void OnMouseMove(MouseMoveEvent& e) noexcept;
	void OnMouseEnter(MouseEnterEvent& e) noexcept;
	void OnMouseLeave(MouseLeaveEvent& e) noexcept;
	void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept;

	std::shared_ptr<Window> m_window;
};
#pragma warning( pop )

// To be defined in CLIENT
Application* CreateApplication();

}