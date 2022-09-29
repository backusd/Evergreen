#pragma once
#include "Core.h"
#include "Events/Event.h"

namespace Evergreen
{
class EVERGREEN_API Application
{
public:
	Application();
	virtual ~Application();

	void Run();

private:

};

// To be defined in CLIENT
Application* CreateApplication();

}