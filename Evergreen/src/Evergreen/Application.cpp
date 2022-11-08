#include "pch.h"
#include "Application.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
#define BIND_EVENT_FN(fn, type) [this](type& e) { this->fn(e); }

Application::Application() noexcept
{
	m_window = std::unique_ptr<Window>(Window::Create());
	m_window->SetOnWindowResize(BIND_EVENT_FN(OnWindowResize, WindowResizeEvent));
	m_window->SetOnWindowCreate(BIND_EVENT_FN(OnWindowCreate, WindowCreateEvent));
	m_window->SetOnWindowClose(BIND_EVENT_FN(OnWindowClose, WindowCloseEvent));
	m_window->SetOnAppTick(BIND_EVENT_FN(OnAppTick, AppTickEvent));
	m_window->SetOnAppUpdate(BIND_EVENT_FN(OnAppUpdate, AppUpdateEvent));
	m_window->SetOnAppRender(BIND_EVENT_FN(OnAppRender, AppRenderEvent));
	m_window->SetOnChar(BIND_EVENT_FN(OnChar, CharEvent));
	m_window->SetOnKeyPressed(BIND_EVENT_FN(OnKeyPressed, KeyPressedEvent));
	m_window->SetOnKeyReleased(BIND_EVENT_FN(OnKeyReleased, KeyReleasedEvent));
	m_window->SetOnMouseMove(BIND_EVENT_FN(OnMouseMove, MouseMoveEvent));
	m_window->SetOnMouseEnter(BIND_EVENT_FN(OnMouseEnter, MouseEnterEvent));
	m_window->SetOnMouseLeave(BIND_EVENT_FN(OnMouseLeave, MouseLeaveEvent));
	m_window->SetOnMouseScrolled(BIND_EVENT_FN(OnMouseScrolled, MouseScrolledEvent));
	m_window->SetOnMouseButtonPressed(BIND_EVENT_FN(OnMouseButtonPressed, MouseButtonPressedEvent));
	m_window->SetOnMouseButtonReleased(BIND_EVENT_FN(OnMouseButtonReleased, MouseButtonReleasedEvent));
	m_window->SetOnMouseButtonDoubleClick(BIND_EVENT_FN(OnMouseButtonDoubleClick, MouseButtonDoubleClickEvent));
}

Application::~Application() noexcept
{
}

int Application::Run() noexcept
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = m_window->ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		m_window->OnUpdate();
	}
}


void Application::OnWindowResize(WindowResizeEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnWindowCreate(WindowCreateEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnWindowClose(WindowCloseEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnAppTick(AppTickEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnAppUpdate(AppUpdateEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnAppRender(AppRenderEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnChar(CharEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnMouseMove(MouseMoveEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnMouseEnter(MouseEnterEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnMouseLeave(MouseLeaveEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnMouseScrolled(MouseScrolledEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}
void Application::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	EG_CORE_INFO(e.ToString());
}

}