#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/UI/Controls/Text.h"


namespace Evergreen
{
class EVERGREEN_API WindowsText : public Text
{
public:
	WindowsText(const std::string& text = "") noexcept;
	WindowsText(const WindowsText& text) noexcept;
	void operator=(const WindowsText&) noexcept;
	virtual ~WindowsText() noexcept {}


};
}