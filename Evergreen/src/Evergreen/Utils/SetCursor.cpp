#include "pch.h"
#include "SetCursor.h"

namespace Evergreen
{
	bool SetCursor(Evergreen::Cursor cursor) noexcept
	{
#ifdef EG_PLATFORM_WINDOWS
		LPCSTR cursorType = IDC_ARROW;
		switch (cursor)
		{
		case Cursor::ARROW:						cursorType = IDC_ARROW; break;
		case Cursor::ARROW_AND_HOURGLASS:		cursorType = IDC_APPSTARTING; break;
		case Cursor::ARROW_AND_QUESTION_MARK:	cursorType = IDC_HELP; break;
		case Cursor::CROSS:						cursorType = IDC_CROSS; break;
		case Cursor::DOUBLE_ARROW_EW:			cursorType = IDC_SIZEWE; break;
		case Cursor::DOUBLE_ARROW_NS:			cursorType = IDC_SIZENS; break;
		case Cursor::DOUBLE_ARROW_NESW:			cursorType = IDC_SIZENESW; break;
		case Cursor::DOUBLE_ARROW_NWSE:			cursorType = IDC_SIZENWSE; break;
		case Cursor::HAND:						cursorType = IDC_HAND; break;
		case Cursor::HOURGLASS:					cursorType = IDC_WAIT; break;
		case Cursor::I_BEAM:					cursorType = IDC_IBEAM; break;
		case Cursor::QUAD_ARROW:				cursorType = IDC_SIZEALL; break;
		case Cursor::SLASHED_CIRCLE:			cursorType = IDC_NO; break;
		case Cursor::UP_ARROW:					cursorType = IDC_UPARROW; break;
		}

		HCURSOR h = LoadCursor(nullptr, cursorType);
		if (h == NULL)
		{
			EG_CORE_WARN("Failed to set cursor to {}", cursor);
			EG_CORE_WARN("    GetLastError() -> {}", TranslateErrorCode(GetLastError()));
			return false;
		}

		SetCursor(h);
		return true;
#else
#error Evergreen only supports Windows
#endif // EG_PLATFORM_WINDOWS
	}
}