#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "WindowTemplate.h"
#include "Evergreen/Events/KeyCode.h"
#include "WindowProperties.h"
#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Events/KeyEvent.h"
#include "Evergreen/Events/MouseEvent.h"

namespace Evergreen
{
enum class Cursor
{
	ARROW = 0,
	ARROW_AND_HOURGLASS = 1,
	ARROW_AND_QUESTION_MARK = 2,
	CROSS = 3,
	DOUBLE_ARROW_EW = 4,
	DOUBLE_ARROW_NS = 5,
	DOUBLE_ARROW_NESW = 6,
	DOUBLE_ARROW_NWSE = 7,
	HAND = 8,
	HOURGLASS = 9,
	I_BEAM = 10,
	QUAD_ARROW = 11,
	SLASHED_CIRCLE = 12,
	UP_ARROW = 13
};

class Window : public WindowTemplate<Window>
{
public:
	Window(const WindowProperties& props = WindowProperties()) noexcept;
	Window(const Window&) = delete;
	void operator=(const Window&) = delete;
	virtual ~Window();

	std::optional<int> ProcessMessages() const noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	void OnUpdate() noexcept;

	inline unsigned int GetWidth() const noexcept { return m_width; }
	inline unsigned int GetHeight() const noexcept { return m_height; }

	// Event Callback Setters
	void SetOnWindowResize(const std::function<void(WindowResizeEvent& e)>& f) noexcept { OnWindowResizeFn = f; }
	void SetOnWindowCreate(const std::function<void(WindowCreateEvent& e)>& f) noexcept { OnWindowCreateFn = f; }
	void SetOnWindowClose(const std::function<void(WindowCloseEvent& e)>& f) noexcept { OnWindowCloseFn = f; }
	void SetOnAppTick(const std::function<void(AppTickEvent& e)>& f) noexcept { OnAppTickFn = f; }
	void SetOnAppUpdate(const std::function<void(AppUpdateEvent& e)>& f) noexcept { OnAppUpdateFn = f; }
	void SetOnAppRender(const std::function<void(AppRenderEvent& e)>& f) noexcept { OnAppRenderFn = f; }
	void SetOnChar(const std::function<void(CharEvent& e)>& f) noexcept { OnCharFn = f; }
	void SetOnKeyPressed(const std::function<void(KeyPressedEvent& e)>& f) noexcept { OnKeyPressedFn = f; }
	void SetOnKeyReleased(const std::function<void(KeyReleasedEvent& e)>& f) noexcept { OnKeyReleasedFn = f; }
	void SetOnMouseMove(const std::function<void(MouseMoveEvent& e)>& f) noexcept { OnMouseMoveFn = f; }
	void SetOnMouseEnter(const std::function<void(MouseEnterEvent& e)>& f) noexcept { OnMouseEnterFn = f; }
	void SetOnMouseLeave(const std::function<void(MouseLeaveEvent& e)>& f) noexcept { OnMouseLeaveFn = f; }
	void SetOnMouseScrolled(const std::function<void(MouseScrolledEvent& e)>& f) noexcept { OnMouseScrolledFn = f; }
	void SetOnMouseButtonPressed(const std::function<void(MouseButtonPressedEvent& e)>& f) noexcept { OnMouseButtonPressedFn = f; }
	void SetOnMouseButtonReleased(const std::function<void(MouseButtonReleasedEvent& e)>& f) noexcept { OnMouseButtonReleasedFn = f; }
	void SetOnMouseButtonDoubleClick(const std::function<void(MouseButtonDoubleClickEvent& e)>& f) noexcept { OnMouseButtonDoubleClickFn = f; }

private:
	bool m_mouseIsInWindow;

	virtual void Init(const WindowProperties& props) noexcept;
	virtual void Shutdown() noexcept;

	KEY_CODE WParamToKeyCode(WPARAM wParam) const noexcept;

	LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnClose(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnRButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept; // cannot be const because it modifies m_mouseIsInWindow
	LRESULT OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnSysKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnSysKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnKillFocus(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;

protected:
	// Application Events
	std::function<void(WindowResizeEvent& e)> OnWindowResizeFn = [](WindowResizeEvent&) {};
	std::function<void(WindowCreateEvent& e)> OnWindowCreateFn = [](WindowCreateEvent&) {};
	std::function<void(WindowCloseEvent& e)> OnWindowCloseFn = [](WindowCloseEvent&) {};
	std::function<void(AppTickEvent& e)> OnAppTickFn = [](AppTickEvent&) {};
	std::function<void(AppUpdateEvent& e)> OnAppUpdateFn = [](AppUpdateEvent&) {};
	std::function<void(AppRenderEvent& e)> OnAppRenderFn = [](AppRenderEvent&) {};

	// Key Events
	std::function<void(CharEvent& e)> OnCharFn = [](CharEvent&) {};
	std::function<void(KeyPressedEvent& e)> OnKeyPressedFn = [](KeyPressedEvent&) {};
	std::function<void(KeyReleasedEvent& e)> OnKeyReleasedFn = [](KeyReleasedEvent&) {};

	// Mouse Events
	std::function<void(MouseMoveEvent& e)> OnMouseMoveFn = [](MouseMoveEvent&) {};
	std::function<void(MouseEnterEvent& e)> OnMouseEnterFn = [](MouseEnterEvent&) {};
	std::function<void(MouseLeaveEvent& e)> OnMouseLeaveFn = [](MouseLeaveEvent&) {};
	std::function<void(MouseScrolledEvent& e)> OnMouseScrolledFn = [](MouseScrolledEvent&) {};
	std::function<void(MouseButtonPressedEvent& e)> OnMouseButtonPressedFn = [](MouseButtonPressedEvent&) {};
	std::function<void(MouseButtonReleasedEvent& e)> OnMouseButtonReleasedFn = [](MouseButtonReleasedEvent&) {};
	std::function<void(MouseButtonDoubleClickEvent& e)> OnMouseButtonDoubleClickFn = [](MouseButtonDoubleClickEvent&) {};

public:
	void InitializeCursors() noexcept;
	static void SetCursor(Cursor cursor) noexcept;
private:
	static const std::array<HCURSOR, 14> m_cursors;
};



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