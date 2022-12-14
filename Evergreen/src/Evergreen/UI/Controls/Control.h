#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/Rendering/DeviceResources.h"

namespace Evergreen
{
struct EVERGREEN_API Margin
{
	float Left;
	float Top;
	float Right;
	float Bottom;
};

// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Control
{
public:
	Control(std::shared_ptr<DeviceResources> deviceResources, 
		    const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		    const Evergreen::Margin& margin = { 0 }) noexcept;
	Control(const Control& control) noexcept;
	void operator=(const Control& control) noexcept;
	virtual ~Control() noexcept {}

	virtual void Update() noexcept = 0;
	virtual void Render() const = 0;

	ND inline std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }

	virtual void OnChar(CharEvent& e) noexcept {}
	virtual void OnKeyPressed(KeyPressedEvent& e) noexcept {}
	virtual void OnKeyReleased(KeyReleasedEvent& e) noexcept {}
	virtual void OnMouseMove(MouseMoveEvent& e) noexcept {}
	virtual void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept {}
	virtual void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept {}
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept {}
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept {}
	virtual void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept {}

	void Name(const std::string& name) noexcept { m_name = name; OnNameChanged(); }
	void Margin(const Evergreen::Margin& margin) noexcept { m_margin = margin; OnMarginChanged(); }
	void Margin(float left, float top, float right, float bottom) noexcept;
	void MarginLeft(float left) noexcept { m_margin.Left = left; OnMarginChanged(); }
	void MarginTop(float top) noexcept { m_margin.Top = top; OnMarginChanged(); }
	void MarginRight(float right) noexcept { m_margin.Right = right; OnMarginChanged(); }
	void MarginBottom(float bottom) noexcept { m_margin.Bottom = bottom; OnMarginChanged();}
	void AllowedRegion(D2D1_RECT_F region) noexcept { m_allowedRegion = region; OnAllowedRegionChanged(); }
	void AllowedRegion(float left, float top, float right, float bottom) noexcept;

	ND inline const std::string& Name() const noexcept { return m_name; }
	ND inline Evergreen::Margin Margin() const noexcept { return m_margin; }
	ND inline float MarginLeft() const noexcept { return m_margin.Left; }
	ND inline float MarginTop() const noexcept { return m_margin.Top; }
	ND inline float MarginRight() const noexcept { return m_margin.Right; }
	ND inline float MarginBottom() const noexcept { return m_margin.Bottom; }
	ND inline D2D1_RECT_F AllowedRegion() const noexcept { return m_allowedRegion; }

protected:
	// On* functions allow derived controls to perform necessary additional actions when a base class method is called
	virtual void OnNameChanged() noexcept {}
	virtual void OnMarginChanged() {}
	virtual void OnAllowedRegionChanged() {}

	std::string							m_name;
	std::shared_ptr<DeviceResources>	m_deviceResources;
	Evergreen::Margin					m_margin;	

	// Allowed region should be set by the parent layout
	D2D1_RECT_F							m_allowedRegion;

};
#pragma warning ( pop )
}
