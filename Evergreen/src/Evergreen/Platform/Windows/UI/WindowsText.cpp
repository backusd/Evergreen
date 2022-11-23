#include "pch.h"
#include "WindowsText.h"



namespace Evergreen
{
// Create will create a WindowsText object
Text* Text::Create(const std::string& text) noexcept
{
	return new WindowsText(text);
}

// ---------------------------------------------------------------------------

WindowsText::WindowsText(const std::string& text = "") noexcept
{

}
WindowsText::WindowsText(const WindowsText& text) noexcept
{

}
void WindowsText::operator=(const WindowsText&) noexcept
{

}


}