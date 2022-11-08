#pragma once
#include "Evergreen/Window.h"
#include "WindowsWindowTemplate.h"
#include "Evergreen/Events/KeyCode.h"

namespace Evergreen
{

class WindowsWindow : public Window, public WindowsWindowTemplate<WindowsWindow>
{
public:
	WindowsWindow(const WindowProperties& props) noexcept;
	WindowsWindow(const WindowsWindow&) = delete;
	void operator=(const WindowsWindow&) = delete;
	virtual ~WindowsWindow();

	std::optional<int> ProcessMessages() const noexcept override;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	void OnUpdate() noexcept override;

	inline unsigned int GetWidth() const noexcept override { return m_width; }
	inline unsigned int GetHeight() const noexcept override { return m_height; }

	inline void SetEventCallback(const EventCallbackFn& callback) noexcept override { EventCallback = callback; }
	//void SetVSync(bool enabled) noexcept override;
	//bool IsVSync() const noexcept override;


private:
	EventCallbackFn EventCallback;

	virtual void Init(const WindowProperties& props) noexcept;
	virtual void Shutdown() noexcept;

	KEY_CODE WParamToKeyCode(WPARAM wParam) const noexcept;

	LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnClose(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;

	LRESULT OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;

	LRESULT OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;

	LRESULT OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnSysKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;
	LRESULT OnSysKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;

	LRESULT OnKillFocus(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept;


};















}