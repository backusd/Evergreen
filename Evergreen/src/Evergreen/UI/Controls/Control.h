#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Rendering/DeviceResources.h"

namespace Evergreen
{
	struct Margin
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
	Control(std::shared_ptr<DeviceResources> deviceResources) noexcept;
	Control(const Control& control) noexcept;
	void operator=(const Control& control) noexcept;
	virtual ~Control() noexcept {}

	virtual void Render() const noexcept = 0;

	virtual void OnMouseMove(MouseMoveEvent& e) noexcept {}
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept {}
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept {}

	void Margin(const Evergreen::Margin& margin) noexcept { m_margin = margin; }
	void Margin(float left, float top, float right, float bottom) noexcept;
	void MarginLeft(float left) noexcept { m_margin.Left = left; }
	void MarginTop(float top) noexcept { m_margin.Top = top; }
	void MarginRight(float right) noexcept { m_margin.Right = right; }
	void MarginBottom(float bottom) noexcept { m_margin.Bottom = bottom; }

	Evergreen::Margin Margin() const noexcept { return m_margin; }
	float MarginLeft() const noexcept { return m_margin.Left; }
	float MarginTop() const noexcept { return m_margin.Top; }
	float MarginRight() const noexcept { return m_margin.Right; }
	float MarginBottom() const noexcept { return m_margin.Bottom; }

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	Evergreen::Margin m_margin;


};
#pragma warning ( pop )
}
