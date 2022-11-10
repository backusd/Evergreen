#pragma once
#include "pch.h"
#include "Core.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"

#ifdef EG_DX11
#include "Evergreen/Rendering/DX11/DeviceResourcesDX11.h"
#elif EG_DX12
#include "Evergreen/Rendering/DX12/DeviceResourcesDX12.h"
#elif EG_OPENGL
#include "Evergreen/Rendering/OpenGL/DeviceResourcesOpenGL.h"
#elif EG_VULKAN
#include "Evergreen/Rendering/Vulkan/DeviceResourcesVulkan.h"
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
#ifdef EG_DX11
	std::unique_ptr<DeviceResourcesDX11> m_deviceResources;
#elif EG_DX12
	std::unique_ptr<DeviceResourcesDX12> m_deviceResources;
#elif EG_OPENGL
	std::unique_ptr<DeviceResourcesOpenGL> m_deviceResources;
#elif EG_VULKAN
	std::unique_ptr<DeviceResourcesVulkan> m_deviceResources;
#endif

};
#pragma warning( pop )

// To be defined in CLIENT
Application* CreateApplication();

}