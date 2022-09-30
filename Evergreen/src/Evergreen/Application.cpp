#include "Application.h"

#include "Evergreen/Events/ApplicationEvent.h"
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

	while (true);
}

}