#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"


#ifdef EG_PLATFORM_WINDOWS
#include "Evergreen/Platform/Windows/TranslateErrorCode.h"
#else
#error Evergreen only supports Windows
#endif // EG_PLATFORM_WINDOWS

namespace Evergreen
{
	enum class Cursor
	{
		ARROW,
		ARROW_AND_HOURGLASS,
		ARROW_AND_QUESTION_MARK,
		CROSS,
		DOUBLE_ARROW_EW,
		DOUBLE_ARROW_NS,
		DOUBLE_ARROW_NESW,
		DOUBLE_ARROW_NWSE,
		HAND,
		HOURGLASS,
		I_BEAM,
		QUAD_ARROW,
		SLASHED_CIRCLE,
		UP_ARROW
	};


	bool SetCursor(Evergreen::Cursor cursor) noexcept;


}

template <>
struct std::formatter<Evergreen::Cursor> : std::formatter<std::string> {
	auto format(Evergreen::Cursor cursor, std::format_context& ctx) {
		std::string s = "";
		switch (cursor)
		{
		case Evergreen::Cursor::ARROW:						s = "Cursor::ARROW"; break;
		case Evergreen::Cursor::ARROW_AND_HOURGLASS:		s = "Cursor::ARROW_AND_HOURGLASS"; break;
		case Evergreen::Cursor::ARROW_AND_QUESTION_MARK:	s = "Cursor::ARROW_AND_QUESTION_MARK"; break;
		case Evergreen::Cursor::CROSS:						s = "Cursor::CROSS"; break;
		case Evergreen::Cursor::DOUBLE_ARROW_EW:			s = "Cursor::DOUBLE_ARROW_EW"; break;
		case Evergreen::Cursor::DOUBLE_ARROW_NS:			s = "Cursor::DOUBLE_ARROW_NS"; break;
		case Evergreen::Cursor::DOUBLE_ARROW_NESW:			s = "Cursor::DOUBLE_ARROW_NESW"; break;
		case Evergreen::Cursor::DOUBLE_ARROW_NWSE:			s = "Cursor::DOUBLE_ARROW_NWSE"; break;
		case Evergreen::Cursor::HAND:						s = "Cursor::HAND"; break;
		case Evergreen::Cursor::HOURGLASS:					s = "Cursor::HOURGLASS"; break;
		case Evergreen::Cursor::I_BEAM:						s = "Cursor::I_BEAM"; break;
		case Evergreen::Cursor::QUAD_ARROW:					s = "Cursor::QUAD_ARROW"; break;
		case Evergreen::Cursor::SLASHED_CIRCLE:				s = "Cursor::SLASHED_CIRCLE"; break;
		case Evergreen::Cursor::UP_ARROW:					s = "Cursor::UP_ARROW"; break;
		default:
			s = "Unrecognized Cursor Type";
			break;
		}
		return formatter<std::string>::format(s, ctx);
	}
};