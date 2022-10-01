#include "pch.h"
#include "Application.h"

#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Events/KeyEvent.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
Application::Application() noexcept
{
}

Application::~Application() noexcept
{
}

void Application::Run() noexcept
{
	WindowResizeEvent e(1280, 720);
	EG_TRACE(e);

	KeyPressedEvent k(static_cast<int>('c'), 0);
	EG_TRACE(k);

	KeyReleasedEvent k2(static_cast<int>('d'));
	EG_TRACE(k2);

	while (true);
}

}