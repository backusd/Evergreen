#include "pch.h"
#include "Application.h"

#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Events/KeyEvent.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application() noexcept
{
	m_window = std::unique_ptr<Window>(Window::Create());
	m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

Application::~Application() noexcept
{
}

void Application::OnEvent(Event& e) noexcept
{
	switch (e.GetEventType())
	{
	case EventType::Character:
		EG_CORE_INFO(e.ToString());
		break;
	case EventType::KeyPressed:
		EG_CORE_INFO(e.ToString());
		break;
	}
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

}