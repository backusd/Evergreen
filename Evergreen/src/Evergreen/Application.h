#pragma once
#include "pch.h"
#include "Core.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"

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
	virtual ~Application() noexcept;

	int Run() noexcept;

private:
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
	void OnMouseScrolled(MouseScrolledEvent& e) noexcept;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept;


	std::unique_ptr<Window> m_window;
};
#pragma warning( pop )

// To be defined in CLIENT
Application* CreateApplication();

}