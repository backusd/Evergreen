#include "pch.h"
#include "WindowsWindow.h"
#include "WindowsMessageMap.h"

#include "Evergreen/Log.h"

namespace Evergreen
{

Window* Window::Create(const WindowProperties& props) noexcept
{
	return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProperties& props) noexcept :
	WindowsWindowTemplate(props)
{
	Init(props);
}

WindowsWindow::~WindowsWindow()
{
	Shutdown();
}

void WindowsWindow::Shutdown() noexcept
{
}

void WindowsWindow::Init(const WindowProperties& props) noexcept
{
	EG_CORE_INFO("Creating window {0} ({1}, {2})", m_title, m_width, m_height);


	// ... Create window ...
}

std::optional<int> WindowsWindow::ProcessMessages() const noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return static_cast<int>(msg.wParam);
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);

		// Can optionally obtain the LRESULT value that is returned, but from the Microsoft docs:
		// "The return value specifies the value returned by the window procedure. Although its 
		// meaning depends on the message being dispatched, the return value generally is ignored."
		// LRESULT result = DispatchMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

LRESULT WindowsWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// static WindowsMessageMap mm;
	// EG_CORE_TRACE(mm(msg, wParam, lParam));

	switch (msg)
	{
	case WM_CREATE:			return OnCreate(hWnd, msg, wParam, lParam);
	case WM_CLOSE:			return OnClose(hWnd, msg, wParam, lParam);
	case WM_DESTROY:		return OnDestroy(hWnd, msg, wParam, lParam);
	case WM_LBUTTONDOWN:	return OnLButtonDown(hWnd, msg, wParam, lParam);
	case WM_LBUTTONUP:		return OnLButtonUp(hWnd, msg, wParam, lParam);
	case WM_LBUTTONDBLCLK:	return OnLButtonDoubleClick(hWnd, msg, wParam, lParam);
	case WM_MBUTTONDOWN:	return OnMButtonDown(hWnd, msg, wParam, lParam);
	case WM_MBUTTONUP:		return OnMButtonUp(hWnd, msg, wParam, lParam);
	case WM_RBUTTONDOWN:	return OnRButtonDown(hWnd, msg, wParam, lParam);
	case WM_RBUTTONUP:		return OnRButtonUp(hWnd, msg, wParam, lParam);
	case WM_PAINT:			return OnPaint(hWnd, msg, wParam, lParam);
	case WM_SIZE:			return OnResize(hWnd, msg, wParam, lParam);
	case WM_MOUSEMOVE:		return OnMouseMove(hWnd, msg, wParam, lParam);
	case WM_MOUSELEAVE:		return OnMouseLeave(hWnd, msg, wParam, lParam);
	case WM_MOUSEWHEEL:		return OnMouseWheel(hWnd, msg, wParam, lParam);
	case WM_GETMINMAXINFO:	return OnGetMinMaxInfo(hWnd, msg, wParam, lParam);
	case WM_CHAR:			return OnChar(hWnd, msg, wParam, lParam);
	case WM_SYSKEYUP:		return OnSysKeyUp(hWnd, msg, wParam, lParam);
	case WM_KEYUP:			return OnKeyUp(hWnd, msg, wParam, lParam);
	case WM_SYSKEYDOWN:		return OnSysKeyDown(hWnd, msg, wParam, lParam);
	case WM_KEYDOWN:		return OnKeyDown(hWnd, msg, wParam, lParam);
	case WM_KILLFOCUS:		return OnKillFocus(hWnd, msg, wParam, lParam);
	case WM_DPICHANGED:
		/*
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			//const int dpi = HIWORD(wParam);
			//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		*/
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



void WindowsWindow::OnUpdate() noexcept
{
	// ... Update + Render + Present ... ??
}

/*
void WindowsWindow::SetVSync(bool enabled) noexcept
{
	m_data.vSync = enabled;

	// ... Not sure if I need/want this ... ??
}

bool WindowsWindow::IsVSync() const noexcept
{
	return m_data.vSync;
}
*/


LRESULT WindowsWindow::OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnClose(HWND /* hWnd */, UINT /* msg */, WPARAM /* wParam */, LPARAM /* lParam */) const noexcept
{
	// we don't want the DefProc to handle this message because
	// we want our destructor to destroy the window, so return 0 instead of break

	// Don't post quit - Just set the destroy window flag
	// WindowManager will worry about posting WM_QUIT
	// m_destroy = true;

	/* Perform any clean up work here */

	PostQuitMessage(0);

	return 0;
}

LRESULT WindowsWindow::OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		Mouse::OnLeftPressed(pt.x, pt.y);
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		Mouse::OnLeftReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
		{
			ReleaseCapture();
			Mouse::OnMouseLeave();
		}
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		Mouse::OnLeftDoubleClick(pt.x, pt.y);
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		Mouse::OnMiddlePressed(pt.x, pt.y);
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		Mouse::OnMiddleReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
		{
			ReleaseCapture();
			Mouse::OnMouseLeave();
		}
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		Mouse::OnRightPressed(pt.x, pt.y);
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		Mouse::OnRightReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
		{
			ReleaseCapture();
			Mouse::OnMouseLeave();
		}
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnPaint(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*
	m_width = LOWORD(lParam);
	m_height = HIWORD(lParam);

	// For now, just create the viewport for the whole screen
	CD3D11_VIEWPORT vp = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height));

	DeviceResources::OnResize();
	m_simulationRenderer->OnResize(vp);
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// in client region -> log move, and log enter + capture mouse (if not previously in window)
		if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height)
		{
			Mouse::OnMouseMove(pt.x, pt.y);
			if (!Mouse::IsInWindow()) // IsInWindow() will tell you if it was PREVIOUSLY in the window or not
			{
				SetCapture(hWnd);
				Mouse::OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
			{
				Mouse::OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else
			{
				ReleaseCapture();
				Mouse::OnMouseLeave();
			}
		}
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowsWindow::OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		Mouse::OnWheelDelta(pt.x, pt.y, delta);
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnGetMinMaxInfo(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		Keyboard::OnChar(static_cast<unsigned char>(wParam));
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		Keyboard::OnKeyReleased(static_cast<unsigned char>(wParam));
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	if (!m_io.WantCaptureMouse)
	{
		if (!(lParam & 0x40000000) || Keyboard::AutorepeatIsEnabled()) // filter autorepeat
		{
			Keyboard::OnKeyPressed(static_cast<unsigned char>(wParam));
		}
	}
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnSysKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	return this->OnKeyUp(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnSysKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	return this->OnKeyDown(hWnd, msg, wParam, lParam);
}

LRESULT WindowsWindow::OnKillFocus(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const noexcept
{
	/*
	// clear keystate when window loses focus to prevent input getting "stuck"
	Keyboard::ClearState();
	*/
	return DefWindowProc(hWnd, msg, wParam, lParam);
}







}