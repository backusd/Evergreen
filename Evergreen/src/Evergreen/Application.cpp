#include "pch.h"
#include "Application.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
#define BIND_EVENT_FN(fn, type) [this](type& e) { this->fn(e); }

Application::Application() noexcept
{
	// Create main window
	m_window = std::shared_ptr<Window>(Window::Create());
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

	// Create DeviceResources
#ifdef EG_DX11
	m_deviceResources = std::make_unique<DeviceResourcesDX11>(m_window.get());
#elif EG_DX12
	m_deviceResources = std::make_unique<DeviceResourcesDX12>(m_window.get());
#elif EG_OPENGL
	m_deviceResources = std::make_unique<DeviceResourcesOpenGL>(m_window.get());
#elif EG_VULKAN
	m_deviceResources = std::make_unique<DeviceResourcesVulkan>(m_window.get());
#endif

	m_ui = std::make_unique<UI>(m_window);

	/*
	m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()));
	m_rootLayout->Name("root layout");
	
	std::optional<Row*> row = m_rootLayout->AddRow({ RowColumnType::FIXED, 60.0f });
	if (row.has_value())
	{
		row.value()->BottomIsAdjustable(true);
		row.value()->MaxHeight(100.0f);
		row.value()->MinHeight(30.0f);
	}

	row = m_rootLayout->AddRow({ RowColumnType::PERCENT, 0.5f });
	if (row.has_value())
	{
		row.value()->TopIsAdjustable(true);
	}

	m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f });
	m_rootLayout->AddRow({ RowColumnType::STAR, 2.0f });

	m_rootLayout->AddColumn({ RowColumnType::PERCENT, 0.5f });
	m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	// LayoutCheck is entirely optional - In a Release build, this does nothing
	m_rootLayout->LayoutCheck();
	*/
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

		Update();
		Render();
		Present();
	}
}

void Application::Update() noexcept
{
	m_window->OnUpdate();
}

void Application::Render() noexcept
{
	// Must always start the render by clearing the background
	// NOTE: Do not need to reset the render target - DeviceResources handles that itself
//	m_deviceResources->ClearBackground({1.0f, 1.0f, 0.0f, 1.0f});

	std::optional<const Color> color = Color::GetColor("Chartreuse");
	if (color.has_value())
		m_deviceResources->ClearBackground(color.value());
	else
		m_deviceResources->ClearBackground(Color::Black);

	
	m_ui->Render();
}

void Application::Present() noexcept
{
	m_deviceResources->Present();
}


void Application::OnWindowResize(WindowResizeEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
	m_deviceResources->OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
	m_ui->OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
}
void Application::OnWindowCreate(WindowCreateEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnWindowClose(WindowCloseEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnAppTick(AppTickEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnAppUpdate(AppUpdateEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnAppRender(AppRenderEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnChar(CharEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseMove(MouseMoveEvent& e) noexcept
{
	//EG_CORE_INFO("{}", e);
}
void Application::OnMouseEnter(MouseEnterEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseLeave(MouseLeaveEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseScrolled(MouseScrolledEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}
void Application::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	EG_CORE_INFO("{}", e);
}

}