#pragma once
#include "pch.h"

#include "WindowProperties.h"
#include "Evergreen/Core.h"
#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Events/KeyEvent.h"
#include "Evergreen/Events/MouseEvent.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Window
{
public:
	Window() noexcept = default;
	Window(const Window&) = delete;
	void operator=(const Window&) = delete;	
	virtual ~Window() {}

	virtual void OnUpdate() noexcept = 0;

	virtual unsigned int GetWidth() const noexcept = 0;
	virtual unsigned int GetHeight() const noexcept = 0;

	virtual std::optional<int> ProcessMessages() const noexcept = 0;

	static Window* Create(const WindowProperties& props = WindowProperties()) noexcept;

	// Event Setters
	void SetOnWindowResize(const std::function<void(WindowResizeEvent& e)>& f) noexcept { OnWindowResizeFn = f; }
	void SetOnWindowCreate(const std::function<void(WindowCreateEvent& e)>& f) noexcept { OnWindowCreateFn = f; }
	void SetOnWindowClose(const std::function<void(WindowCloseEvent& e)>& f) noexcept { OnWindowCloseFn = f; }
	void SetOnAppTick(const std::function<void(AppTickEvent& e)>& f) noexcept { OnAppTickFn = f; }
	void SetOnAppUpdate(const std::function<void(AppUpdateEvent& e)>& f) noexcept { OnAppUpdateFn = f; }
	void SetOnAppRender(const std::function<void(AppRenderEvent& e)>& f) noexcept { OnAppRenderFn = f; }
	void SetOnChar(const std::function<void(CharEvent& e)>& f) noexcept { OnCharFn = f; }
	void SetOnKeyPressed(const std::function<void(KeyPressedEvent& e)>& f) noexcept { OnKeyPressedFn = f; }
	void SetOnKeyReleased(const std::function<void(KeyReleasedEvent& e)>& f) noexcept { OnKeyReleasedFn = f; }
	void SetOnMouseMove(const std::function<void(MouseMoveEvent& e)>& f) noexcept { OnMouseMoveFn = f; }
	void SetOnMouseEnter(const std::function<void(MouseEnterEvent& e)>& f) noexcept { OnMouseEnterFn = f; }
	void SetOnMouseLeave(const std::function<void(MouseLeaveEvent& e)>& f) noexcept { OnMouseLeaveFn = f; }
	void SetOnMouseScrolled(const std::function<void(MouseScrolledEvent& e)>& f) noexcept { OnMouseScrolledFn = f; }
	void SetOnMouseButtonPressed(const std::function<void(MouseButtonPressedEvent& e)>& f) noexcept { OnMouseButtonPressedFn = f; }
	void SetOnMouseButtonReleased(const std::function<void(MouseButtonReleasedEvent& e)>& f) noexcept { OnMouseButtonReleasedFn = f; }
	void SetOnMouseButtonDoubleClick(const std::function<void(MouseButtonDoubleClickEvent& e)>& f) noexcept { OnMouseButtonDoubleClickFn = f; }

protected:
	// Application Events
	std::function<void(WindowResizeEvent& e)> OnWindowResizeFn = [](WindowResizeEvent&) {};
	std::function<void(WindowCreateEvent& e)> OnWindowCreateFn = [](WindowCreateEvent&) {};
	std::function<void(WindowCloseEvent& e)> OnWindowCloseFn = [](WindowCloseEvent&) {};
	std::function<void(AppTickEvent& e)> OnAppTickFn = [](AppTickEvent&) {};
	std::function<void(AppUpdateEvent& e)> OnAppUpdateFn = [](AppUpdateEvent&) {};
	std::function<void(AppRenderEvent& e)> OnAppRenderFn = [](AppRenderEvent&) {};

	// Key Events
	std::function<void(CharEvent& e)> OnCharFn = [](CharEvent&) {};
	std::function<void(KeyPressedEvent& e)> OnKeyPressedFn = [](KeyPressedEvent&) {};
	std::function<void(KeyReleasedEvent& e)> OnKeyReleasedFn = [](KeyReleasedEvent&) {};

	// Mouse Events
	std::function<void(MouseMoveEvent& e)> OnMouseMoveFn = [](MouseMoveEvent&) {};
	std::function<void(MouseEnterEvent& e)> OnMouseEnterFn = [](MouseEnterEvent&) {};
	std::function<void(MouseLeaveEvent& e)> OnMouseLeaveFn = [](MouseLeaveEvent&) {};
	std::function<void(MouseScrolledEvent& e)> OnMouseScrolledFn = [](MouseScrolledEvent&) {};
	std::function<void(MouseButtonPressedEvent& e)> OnMouseButtonPressedFn = [](MouseButtonPressedEvent&) {};
	std::function<void(MouseButtonReleasedEvent& e)> OnMouseButtonReleasedFn = [](MouseButtonReleasedEvent&) {};
	std::function<void(MouseButtonDoubleClickEvent& e)> OnMouseButtonDoubleClickFn = [](MouseButtonDoubleClickEvent&) {};
};
#pragma warning( pop )
















}