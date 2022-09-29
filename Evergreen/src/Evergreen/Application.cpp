#include "Application.h"

#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
Application::Application()
{
}

Application::~Application()
{
}

void Application::Run()
{
	WindowResizeEvent e(1280, 720);
	EG_TRACE(e);

	while (true);
}

}