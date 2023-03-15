#include "pch.h"
#include "Application.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
#define BIND_EVENT_FN(fn, type) [this](type& e) { this->fn(e); }

Application::Application()
{
	// Create main window
	m_window = std::make_shared<Window>();
	m_window->InitializeCursors();

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
	m_window->SetOnMouseScrolledVertical(BIND_EVENT_FN(OnMouseScrolledVertical, MouseScrolledEvent));
	m_window->SetOnMouseScrolledHorizontal(BIND_EVENT_FN(OnMouseScrolledHorizontal, MouseScrolledEvent));
	m_window->SetOnMouseButtonPressed(BIND_EVENT_FN(OnMouseButtonPressed, MouseButtonPressedEvent));
	m_window->SetOnMouseButtonReleased(BIND_EVENT_FN(OnMouseButtonReleased, MouseButtonReleasedEvent));
	m_window->SetOnMouseButtonDoubleClick(BIND_EVENT_FN(OnMouseButtonDoubleClick, MouseButtonDoubleClickEvent));

	// Create DeviceResources
	m_deviceResources = std::make_shared<DeviceResources>(m_window.get());

	m_ui = std::make_unique<UI>(m_deviceResources, m_window);
}

int Application::Run()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = m_window->ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		m_timer.Tick([&]()
			{
				Update(m_timer);
			}
		);

		Render();
		Present();
	}
}

void Application::Update(const Timer& timer)
{
	// Call the virtual method first so we can update the simulation, or whatever the
	// client code is needing to update, before we update the window/UI
	OnUpdate(timer);

	m_ui->Update(timer);
}

void Application::Render()
{
	// Before rendering the UI, call the virtual OnRender method so we can render the simulation, 
	// or whatever the client code is needs to render, before we render the UI on top of it
	OnRender();
	
	m_ui->Render();
}

void Application::Present()
{
	m_deviceResources->Present();
}


void Application::OnWindowResize(WindowResizeEvent& e)
{
	EG_CORE_INFO("{}", e);
	m_deviceResources->OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
	m_ui->OnWindowResize(e);
}
void Application::OnWindowCreate(WindowCreateEvent& e)
{
	EG_CORE_INFO("{}", e);
}
void Application::OnWindowClose(WindowCloseEvent& e)
{
	EG_CORE_INFO("{}", e);
}
void Application::OnAppTick(AppTickEvent& e)
{
	EG_CORE_INFO("{}", e);
}
void Application::OnAppUpdate(AppUpdateEvent& e)
{
	EG_CORE_INFO("{}", e);
}
void Application::OnAppRender(AppRenderEvent& e)
{
	EG_CORE_INFO("{}", e);
}
void Application::OnChar(CharEvent& e)
{
	EG_CORE_INFO("{}", e);
	m_ui->OnChar(e);
}
void Application::OnKeyPressed(KeyPressedEvent& e)
{
	EG_CORE_INFO("{}", e);
	m_ui->OnKeyPressed(e);
}
void Application::OnKeyReleased(KeyReleasedEvent& e)
{
	EG_CORE_INFO("{}", e);
	m_ui->OnKeyReleased(e);
}
void Application::OnMouseMove(MouseMoveEvent& e)
{
	m_ui->OnMouseMove(e);
	//EG_CORE_INFO("{}", e);
}
void Application::OnMouseEnter(MouseEnterEvent& e)
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseLeave(MouseLeaveEvent& e)
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseScrolledVertical(MouseScrolledEvent& e)
{
	m_ui->OnMouseScrolledVertical(e);
}
void Application::OnMouseScrolledHorizontal(MouseScrolledEvent& e)
{
	m_ui->OnMouseScrolledHorizontal(e);
}
void Application::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	m_ui->OnMouseButtonPressed(e);
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	m_ui->OnMouseButtonReleased(e);
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e)
{
	m_ui->OnMouseButtonDoubleClick(e);
	EG_CORE_INFO("{}", e);
}

}