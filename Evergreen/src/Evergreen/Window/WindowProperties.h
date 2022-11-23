#pragma once
#include "pch.h"


namespace Evergreen
{
struct WindowProperties
{
	std::string title;
	unsigned int width;
	unsigned int height;

	WindowProperties(const std::string& title = "Evergreen Application",
		unsigned int width = 1280,
		unsigned int height = 720) noexcept :
		title(title), width(width), height(height)
	{}
};





}