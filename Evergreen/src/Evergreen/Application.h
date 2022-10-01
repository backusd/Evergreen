#pragma once
#include "pch.h"
#include "Core.h"
#include "Events/Event.h"

namespace Evergreen
{
class EVERGREEN_API Application
{
public:
	Application() noexcept;
	virtual ~Application() noexcept;

	void Run() noexcept;

private:

};

// To be defined in CLIENT
Application* CreateApplication();

}