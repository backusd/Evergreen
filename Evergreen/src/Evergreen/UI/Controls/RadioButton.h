#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/Events/RadioButtonEvents.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/UI/Layout.h"


// Macro for declaring a derived class
#define RADIOBUTTON(name)	class name : public Evergreen::RadioButton																						\
							{																																\
							public:																															\
								name(std::shared_ptr<Evergreen::DeviceResources> deviceResources,															\
									Evergreen::UI* ui,																										\
									const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),											\
									bool isChecked = false,																									\
									float outerRadius = 6.0f,																								\
									float innerRadius = 3.0f,																								\
									std::unique_ptr<Evergreen::ColorBrush> outerBrush = nullptr,															\
									std::unique_ptr<Evergreen::ColorBrush> innerBrush = nullptr,															\
									const Evergreen::Margin& margin = { 0 }) noexcept :																		\
									Evergreen::RadioButton(deviceResources, ui, allowedRegion, isChecked, outerRadius, innerRadius, 						\
										std::move(outerBrush), std::move(innerBrush), margin)																\
								{}																															\
								name(const name&) noexcept = delete;																						\
								name& operator=(const name&) noexcept = delete;																				\
								virtual ~name() noexcept override {}																						\
																																							\
								virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;														\
								virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;														\
								virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;														\
								virtual void HandleOnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;										\
								virtual void HandleOnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;											\
								virtual void HandleOnIsCheckedChanged(Evergreen::RadioButtonIsCheckedChangedEvent&) override;								\
							};																																\


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API RadioButton : public Control
{
public:
	RadioButton(std::shared_ptr<DeviceResources> deviceResources,
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		bool isChecked = false,
		float outerRadius = 6.0f,
		float innerRadius = 3.0f,
		std::unique_ptr<ColorBrush> outerBrush = nullptr,
		std::unique_ptr<ColorBrush> innerBrush = nullptr,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	RadioButton(const RadioButton& text) noexcept = delete; // Just delete for now until there is a good use case
	RadioButton& operator=(const RadioButton&) noexcept = delete;
	virtual ~RadioButton() noexcept override {}

	// Event Handlers
	virtual void HandleOnMouseEntered(MouseMoveEvent&) {}
	virtual void HandleOnMouseExited(MouseMoveEvent&) {}
	virtual void HandleOnMouseMove(MouseMoveEvent&) {}
	virtual void HandleOnMouseLButtonDown(MouseButtonPressedEvent&) {}
	virtual void HandleOnMouseLButtonUp(MouseButtonReleasedEvent&) {}
	virtual void HandleOnIsCheckedChanged(RadioButtonIsCheckedChangedEvent&) {}

	// Inherited from Control
	void Render() const override;

	// There is a special case where a Pane has handled a move event, but this Control was previously handling
	// the move events. Therefore, we need to inform this Control that the mouse is no longer over the Control
	void MouseMoveHandledByPane(MouseMoveEvent& e) override;

	// Event handling
	virtual void OnMouseMove(MouseMoveEvent& e) override;
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) override;
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) override;

	// RadioButton specific
	ND inline bool GetIsChecked() const noexcept { return m_isChecked; }
	ND inline ColorBrush* GetOuterBrush() const noexcept { return m_outerBrush.get(); }
	ND inline ColorBrush* GetInnerBrush() const noexcept { return m_innerBrush.get(); }
	ND inline float GetOuterLineWidth() const noexcept { return m_outerLineWidth; }
	ND inline float GetOuterRadius() const noexcept { return m_outerRadius; }
	ND inline float GetInnerRadius() const noexcept { return m_innerRadius; }

	void SetIsChecked(bool checked) noexcept;
	inline void SetOuterBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_outerBrush = std::move(brush); }
	inline void SetInnerBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_innerBrush = std::move(brush); }
	void SetOuterLineWidth(float width) noexcept { m_outerLineWidth = width; }
	void SetInnerRadius(float width) { m_innerRadius = width; RadioButtonChanged(); }
	void SetOuterRadius(float width) { m_outerRadius = width; RadioButtonChanged(); }

	ND inline virtual ControlType GetControlType() const noexcept override { return ControlType::RadioButton; }

protected:
	enum class MouseOverState
	{
		NOT_OVER,
		OVER,
		OVER_AND_LBUTTON_DOWN
	};

	virtual void RadioButtonChanged();
	virtual void OnMarginChanged() override { RadioButtonChanged(); }
	virtual void OnAllowedRegionChanged() override;

	ND inline bool MouseIsOver(float x, float y) noexcept;

	bool						m_isChecked;
	std::unique_ptr<ColorBrush> m_innerBrush;
	std::unique_ptr<ColorBrush> m_outerBrush;
	float						m_outerLineWidth;
	float						m_innerRadius;
	float						m_outerRadius;
	Microsoft::WRL::ComPtr<ID2D1EllipseGeometry> m_innerCircle;
	Microsoft::WRL::ComPtr<ID2D1EllipseGeometry> m_outerCircle;
	MouseOverState				m_mouseState;
};
#pragma warning( pop )

}
