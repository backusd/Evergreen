#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/Rendering/DeviceResources.h"
#include "Evergreen/Utils/Timer.h"

namespace Evergreen
{
struct EVERGREEN_API Margin
{
	float Left;
	float Top;
	float Right;
	float Bottom;
};

class UI;

// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Control
{
public:
	enum class ControlType
	{
		Button,
		RoundedButton,
		Pane,
		RadioButton,
		ScrollableLayout,
		SliderFloat,
		SliderInt,
		Text,
		TextInput,
		Viewport
	};

public:
	Control(std::shared_ptr<DeviceResources> deviceResources, 
			UI* ui,
		    const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		    const Evergreen::Margin& margin = { 0 }) noexcept;
	Control(const Control& control) noexcept = delete;
	Control& operator=(const Control& control) noexcept = delete;
	virtual ~Control() noexcept {}

	void Update(const Timer& timer) 
	{ 
		OnUpdate(timer); 
		m_CustomOnUpdateCallback(this, timer); 
	}
protected:
	virtual void OnUpdate(const Timer& timer) {}
public:
	virtual void Render() const = 0;
	virtual ControlType GetControlType() const noexcept = 0;

	ND inline std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }

	virtual void OnChar(CharEvent& e) {}
	virtual void OnKeyPressed(KeyPressedEvent& e) {}
	virtual void OnKeyReleased(KeyReleasedEvent& e) {}
	virtual void OnMouseMove(MouseMoveEvent& e) {}
	virtual void OnMouseScrolledVertical(MouseScrolledEvent& e) {}
	virtual void OnMouseScrolledHorizontal(MouseScrolledEvent& e) {}
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) {}
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) {}
	virtual void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) {}

	// There is a special case where a Pane has handled a move event, but this Control was previously handling
	// the move events. Therefore, we need to inform this Control that the mouse is no longer over the Control
	virtual void MouseMoveHandledByPane(MouseMoveEvent& e) {}

	void Name(const std::string& name) noexcept { m_name = name; OnNameChanged(); }
	void ID(unsigned int id) noexcept { m_id = id; }
	void Margin(const Evergreen::Margin& margin) noexcept { m_margin = margin; OnMarginChanged(); }
	void Margin(float left, float top, float right, float bottom) noexcept;
	void MarginLeft(float left) noexcept { m_margin.Left = left; OnMarginChanged(); }
	void MarginTop(float top) noexcept { m_margin.Top = top; OnMarginChanged(); }
	void MarginRight(float right) noexcept { m_margin.Right = right; OnMarginChanged(); }
	void MarginBottom(float bottom) noexcept { m_margin.Bottom = bottom; OnMarginChanged();}
	void AllowedRegion(D2D1_RECT_F region) noexcept { m_allowedRegion = region; OnAllowedRegionChanged(); }
	void AllowedRegion(float left, float top, float right, float bottom) noexcept;
	void AllowedRegionLeft(float left) noexcept;
	void AllowedRegionRight(float right) noexcept;
	void AllowedRegionTop(float top) noexcept;
	void AllowedRegionBottom(float bottom) noexcept;
	void SetOnUpdateCallback(std::function<void(Control*, const Timer&)> fn) noexcept { m_CustomOnUpdateCallback = fn; }

	ND inline const std::string& Name() const noexcept { return m_name; }
	ND inline unsigned int ID() const noexcept { return m_id; }
	ND inline const Evergreen::Margin& Margin() const noexcept { return m_margin; }
	ND inline float MarginLeft() const noexcept { return m_margin.Left; }
	ND inline float MarginTop() const noexcept { return m_margin.Top; }
	ND inline float MarginRight() const noexcept { return m_margin.Right; }
	ND inline float MarginBottom() const noexcept { return m_margin.Bottom; }
	ND inline const D2D1_RECT_F& AllowedRegion() const noexcept { return m_allowedRegion; }
	ND inline UI* GetUI() const noexcept { return m_ui; }

	// Virtual functions to retrieve a layout/control by either name or ID. By default, they will simply only
	// check whether their name/ID matches and either return 'this' or nullptr. However, other controls (such
	// as Button) which may contain sub-controls, will want to forward this function call to test its sub-layouts/controls
	ND inline virtual Control* GetControlByName(const std::string& name) noexcept { return m_name.compare(name) == 0 ? this : nullptr; }
	ND inline virtual Control* GetControlByID(unsigned int id) noexcept { return m_id == id ? this : nullptr; }
	ND inline virtual Layout* GetLayoutByName(const std::string& name) noexcept { return nullptr; }
	ND inline virtual Layout* GetLayoutByID(unsigned int id) noexcept { return nullptr; }

protected:
	// On* functions allow derived controls to perform necessary additional actions when a base class method is called
	virtual void OnNameChanged() noexcept {}
	virtual void OnMarginChanged() {}
	virtual void OnAllowedRegionChanged() {}

	ND inline bool AllowedRegionContainsPoint(float x, float y) const noexcept;

	std::string							m_name;
	unsigned int						m_id;
	std::shared_ptr<DeviceResources>	m_deviceResources;
	Evergreen::Margin					m_margin;	
	UI*									m_ui;

	std::function<void(Control*, const Timer&)> m_CustomOnUpdateCallback = [](Control*, const Timer&) {};

	// Allowed region should be set by the parent layout
	D2D1_RECT_F							m_allowedRegion;
};
#pragma warning ( pop )
}
