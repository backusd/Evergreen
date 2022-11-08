#pragma once
#include "pch.h"

namespace Evergreen
{
	enum class MOUSE_BUTTON
	{
		EG_LBUTTON,
		EG_RBUTTON,
		EG_MBUTTON,
		EG_XBUTTON1,
		EG_XBUTTON2
	};
}

template <>
struct std::formatter<Evergreen::MOUSE_BUTTON> : std::formatter<std::string> {
	auto format(Evergreen::MOUSE_BUTTON mb, std::format_context& ctx) {
		std::string s = "";
		switch (mb)
		{
		case Evergreen::MOUSE_BUTTON::EG_LBUTTON: s = "LBUTTON"; break;
		case Evergreen::MOUSE_BUTTON::EG_MBUTTON: s = "MBUTTON"; break;
		case Evergreen::MOUSE_BUTTON::EG_RBUTTON: s = "RBUTTON"; break;
		case Evergreen::MOUSE_BUTTON::EG_XBUTTON1: s = "XBUTTON1"; break;		
		case Evergreen::MOUSE_BUTTON::EG_XBUTTON2: s = "XBUTTON2"; break;
		}

		return formatter<std::string>::format(s, ctx);
	}
};