#pragma once



namespace Evergreen
{
	enum class KEY_CODE
	{
		EG_INVALID,
		EG_LBUTTON,
		EG_MBUTTON,
		EG_RBUTTON,
		EG_CANCEL,
		EG_XBUTTON1,
		EG_XBUTTON2,
		EG_BACKSPACE,
		EG_TAB,
		EG_CLEAR,
		EG_ENTER,
		EG_SHIFT,
		EG_CTRL,
		EG_ALT,
		EG_PAUSE,
		EG_CAPS_LOCK,
		EG_ESC,
		EG_SPACEBAR,
		EG_PAGE_UP,
		EG_PAGE_DOWN,
		EG_END,
		EG_HOME,
		EG_LEFT_ARROW,
		EG_RIGHT_ARROW,
		EG_UP_ARROW,
		EG_DOWN_ARROW,
		EG_SELECT,
		EG_PRINT,
		EG_EXECUTE,
		EG_PRINT_SCREEN,
		EG_INSERT,
		EG_HELP,
		EG_0,
		EG_1,
		EG_2,
		EG_3,
		EG_4,
		EG_5,
		EG_6,
		EG_7,
		EG_8,
		EG_9,
		EG_A,
		EG_B,
		EG_C,
		EG_D,
		EG_E,
		EG_F,
		EG_G,
		EG_H,
		EG_I,
		EG_J,
		EG_K,
		EG_L,
		EG_M,
		EG_N,
		EG_O,
		EG_P,
		EG_Q,
		EG_R,
		EG_S,
		EG_T,
		EG_U,
		EG_V,
		EG_W,
		EG_X,
		EG_Y,
		EG_Z,
		EG_LEFT_WINDOWS,
		EG_RIGHT_WINDOWS,
		EG_APPLICATIONS,
		EG_SLEEP,
		EG_MULTIPLY,
		EG_ADD,
		EG_SEPARATOR,
		EG_SUBTRACT,
		EG_DECIMAL,
		EG_DIVIDE,
		EG_F1,
		EG_F2,
		EG_F3,
		EG_F4,
		EG_F5,
		EG_F6,
		EG_F7,
		EG_F8,
		EG_F9,
		EG_F10,
		EG_F11,
		EG_F12,
		EG_F13,
		EG_F14,
		EG_F15,
		EG_F16,
		EG_F17,
		EG_F18,
		EG_F19,
		EG_F20,
		EG_F21,
		EG_F22,
		EG_F23,
		EG_F24,
		EG_NUM_LOCK,
		EG_SCROLL_LOCK
	};
}

template <>
struct std::formatter<Evergreen::KEY_CODE> : std::formatter<std::string> {
	auto format(Evergreen::KEY_CODE kc, std::format_context& ctx) {
		std::string s = "";
		switch (kc)
		{
		case Evergreen::KEY_CODE::EG_LBUTTON: s = "LBUTTON"; break;
		case Evergreen::KEY_CODE::EG_MBUTTON: s = "MBUTTON"; break;
		case Evergreen::KEY_CODE::EG_RBUTTON: s = "RBUTTON"; break;
		case Evergreen::KEY_CODE::EG_CANCEL: s = "CANCEL"; break;
		case Evergreen::KEY_CODE::EG_XBUTTON1: s = "XBUTTON1"; break;
		case Evergreen::KEY_CODE::EG_XBUTTON2: s = "XBUTTON2"; break;
		case Evergreen::KEY_CODE::EG_BACKSPACE: s = "BACKSPACE"; break;
		case Evergreen::KEY_CODE::EG_TAB: s = "TAB"; break;
		case Evergreen::KEY_CODE::EG_CLEAR: s = "CLEAR"; break;
		case Evergreen::KEY_CODE::EG_ENTER: s = "ENTER"; break;
		case Evergreen::KEY_CODE::EG_SHIFT: s = "SHIFT"; break;
		case Evergreen::KEY_CODE::EG_CTRL: s = "CTRL"; break;
		case Evergreen::KEY_CODE::EG_ALT: s = "ALT"; break;
		case Evergreen::KEY_CODE::EG_PAUSE: s = "PAUSE"; break;
		case Evergreen::KEY_CODE::EG_CAPS_LOCK: s = "CAPS_LOCK"; break;
		case Evergreen::KEY_CODE::EG_ESC: s = "ESC"; break;
		case Evergreen::KEY_CODE::EG_SPACEBAR: s = "SPACEBAR"; break;
		case Evergreen::KEY_CODE::EG_PAGE_UP: s = "PAGE_UP"; break;
		case Evergreen::KEY_CODE::EG_PAGE_DOWN: s = "PAGE_DOWN"; break;
		case Evergreen::KEY_CODE::EG_END: s = "END"; break;
		case Evergreen::KEY_CODE::EG_HOME: s = "HOME"; break;
		case Evergreen::KEY_CODE::EG_LEFT_ARROW: s = "LEFT_ARROW"; break;
		case Evergreen::KEY_CODE::EG_RIGHT_ARROW: s = "RIGHT_ARROW"; break;
		case Evergreen::KEY_CODE::EG_UP_ARROW: s = "UP_ARROW"; break;
		case Evergreen::KEY_CODE::EG_DOWN_ARROW: s = "DOWN_ARROW"; break;
		case Evergreen::KEY_CODE::EG_SELECT: s = "SELECT"; break;
		case Evergreen::KEY_CODE::EG_PRINT: s = "PRINT"; break;
		case Evergreen::KEY_CODE::EG_EXECUTE: s = "EXECUTE"; break;
		case Evergreen::KEY_CODE::EG_PRINT_SCREEN: s = "PRINT_SCREEN"; break;
		case Evergreen::KEY_CODE::EG_INSERT: s = "INSERT"; break;
		case Evergreen::KEY_CODE::EG_HELP: s = "HELP"; break;
		case Evergreen::KEY_CODE::EG_0: s = "0"; break;
		case Evergreen::KEY_CODE::EG_1: s = "1"; break;
		case Evergreen::KEY_CODE::EG_2: s = "2"; break;
		case Evergreen::KEY_CODE::EG_3: s = "3"; break;
		case Evergreen::KEY_CODE::EG_4: s = "4"; break;
		case Evergreen::KEY_CODE::EG_5: s = "5"; break;
		case Evergreen::KEY_CODE::EG_6: s = "6"; break;
		case Evergreen::KEY_CODE::EG_7: s = "7"; break;
		case Evergreen::KEY_CODE::EG_8: s = "8"; break;
		case Evergreen::KEY_CODE::EG_9: s = "9"; break;
		case Evergreen::KEY_CODE::EG_A: s = "A"; break;
		case Evergreen::KEY_CODE::EG_B: s = "B"; break;
		case Evergreen::KEY_CODE::EG_C: s = "C"; break;
		case Evergreen::KEY_CODE::EG_D: s = "D"; break;
		case Evergreen::KEY_CODE::EG_E: s = "E"; break;
		case Evergreen::KEY_CODE::EG_F: s = "F"; break;
		case Evergreen::KEY_CODE::EG_G: s = "G"; break;
		case Evergreen::KEY_CODE::EG_H: s = "H"; break;
		case Evergreen::KEY_CODE::EG_I: s = "I"; break;
		case Evergreen::KEY_CODE::EG_J: s = "J"; break;
		case Evergreen::KEY_CODE::EG_K: s = "K"; break;
		case Evergreen::KEY_CODE::EG_L: s = "L"; break;
		case Evergreen::KEY_CODE::EG_M: s = "M"; break;
		case Evergreen::KEY_CODE::EG_N: s = "N"; break;
		case Evergreen::KEY_CODE::EG_O: s = "O"; break;
		case Evergreen::KEY_CODE::EG_P: s = "P"; break;
		case Evergreen::KEY_CODE::EG_Q: s = "Q"; break;
		case Evergreen::KEY_CODE::EG_R: s = "R"; break;
		case Evergreen::KEY_CODE::EG_S: s = "S"; break;
		case Evergreen::KEY_CODE::EG_T: s = "T"; break;
		case Evergreen::KEY_CODE::EG_U: s = "U"; break;
		case Evergreen::KEY_CODE::EG_V: s = "V"; break;
		case Evergreen::KEY_CODE::EG_W: s = "W"; break;
		case Evergreen::KEY_CODE::EG_X: s = "X"; break;
		case Evergreen::KEY_CODE::EG_Y: s = "Y"; break;
		case Evergreen::KEY_CODE::EG_Z: s = "Z"; break;
		case Evergreen::KEY_CODE::EG_LEFT_WINDOWS: s = "LEFT_WINDOWS"; break;
		case Evergreen::KEY_CODE::EG_RIGHT_WINDOWS: s = "RIGHT_WINDOWS"; break;
		case Evergreen::KEY_CODE::EG_APPLICATIONS: s = "APPLICATIONS"; break;
		case Evergreen::KEY_CODE::EG_SLEEP: s = "SLEEP"; break;
		case Evergreen::KEY_CODE::EG_MULTIPLY: s = "MULTIPLY"; break;
		case Evergreen::KEY_CODE::EG_ADD: s = "ADD"; break;
		case Evergreen::KEY_CODE::EG_SEPARATOR: s = "SEPARATOR"; break;
		case Evergreen::KEY_CODE::EG_SUBTRACT: s = "SUBTRACT"; break;
		case Evergreen::KEY_CODE::EG_DECIMAL: s = "DECIMAL"; break;
		case Evergreen::KEY_CODE::EG_DIVIDE: s = "DIVIDE"; break;
		case Evergreen::KEY_CODE::EG_F1: s = "F1"; break;
		case Evergreen::KEY_CODE::EG_F2: s = "F2"; break;
		case Evergreen::KEY_CODE::EG_F3: s = "F3"; break;
		case Evergreen::KEY_CODE::EG_F4: s = "F4"; break;
		case Evergreen::KEY_CODE::EG_F5: s = "F5"; break;
		case Evergreen::KEY_CODE::EG_F6: s = "F6"; break;
		case Evergreen::KEY_CODE::EG_F7: s = "F7"; break;
		case Evergreen::KEY_CODE::EG_F8: s = "F8"; break;
		case Evergreen::KEY_CODE::EG_F9: s = "F9"; break;
		case Evergreen::KEY_CODE::EG_F10: s = "F10"; break;
		case Evergreen::KEY_CODE::EG_F11: s = "F11"; break;
		case Evergreen::KEY_CODE::EG_F12: s = "F12"; break;
		case Evergreen::KEY_CODE::EG_F13: s = "F13"; break;
		case Evergreen::KEY_CODE::EG_F14: s = "F14"; break;
		case Evergreen::KEY_CODE::EG_F15: s = "F15"; break;
		case Evergreen::KEY_CODE::EG_F16: s = "F16"; break;
		case Evergreen::KEY_CODE::EG_F17: s = "F17"; break;
		case Evergreen::KEY_CODE::EG_F18: s = "F18"; break;
		case Evergreen::KEY_CODE::EG_F19: s = "F19"; break;
		case Evergreen::KEY_CODE::EG_F20: s = "F20"; break;
		case Evergreen::KEY_CODE::EG_F21: s = "F21"; break;
		case Evergreen::KEY_CODE::EG_F22: s = "F22"; break;
		case Evergreen::KEY_CODE::EG_F23: s = "F23"; break;
		case Evergreen::KEY_CODE::EG_F24: s = "F24"; break;
		case Evergreen::KEY_CODE::EG_NUM_LOCK: s = "NUM_LOCK"; break;
		case Evergreen::KEY_CODE::EG_SCROLL_LOCK: s = "SCROLL_LOCK"; break;
		case Evergreen::KEY_CODE::EG_INVALID: s = "Invalid"; break;
		}

		return formatter<std::string>::format(s, ctx);
	}
};