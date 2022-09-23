#pragma once
#include "Core.h"


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