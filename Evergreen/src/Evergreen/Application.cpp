#include "pch.h"
#include "Application.h"

#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Events/KeyEvent.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
Application::Application() noexcept
{
	m_window = std::unique_ptr<Window>(Window::Create());
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

}