#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/UI/Controls/Text.h"
#include "Evergreen/UI/Controls/TextInput.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class SliderFloat : public Control
{
public:
	SliderFloat(std::shared_ptr<DeviceResources> deviceResources,
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		float minValue = 0.0f, float maxValue = 1.0f, float initialValue = 0.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	SliderFloat(const SliderFloat&) noexcept = delete; // Just delete for now until there is a good use case
	void operator=(const SliderFloat&) noexcept = delete;
	virtual ~SliderFloat() noexcept override {}

	// Inherited from Control
	virtual void Update() noexcept override {}
	virtual void Render() const noexcept override;

	// Event Handling
	void OnMouseMove(MouseMoveEvent& e) noexcept override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;
	void OnChar(CharEvent& e) noexcept override { m_valueTextInputOnRight->OnChar(e); }
	void OnKeyPressed(KeyPressedEvent& e) noexcept override { m_valueTextInputOnRight->OnKeyPressed(e); }
	void OnKeyReleased(KeyReleasedEvent& e) noexcept override { m_valueTextInputOnRight->OnKeyReleased(e); }
	void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept override { m_valueTextInputOnRight->OnMouseScrolledVertical(e); }
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept override { m_valueTextInputOnRight->OnMouseScrolledHorizontal(e); }
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept override { m_valueTextInputOnRight->OnMouseButtonDoubleClick(e); }

	float GetMinimumValue() const noexcept { return m_minValue; }
	float GetMaximumValue() const noexcept { return m_maxValue; }

	void SetValueFormatString(const std::wstring& fmt) noexcept { m_valueFormatString = fmt; }
	void SetValue(float value) noexcept;

protected:
	enum class MouseOverCircleState
	{
		NOT_OVER,
		OVER,
		DRAGGING
	};

	void SliderChanged();
	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;

	ND float ValueToPixel() const noexcept;
	ND float PixelToValue(float pixel) const noexcept;

	ND inline bool CircleContainsPoint(float x, float y) const noexcept;

	ND inline D2D1_RECT_F GetMinTextAllowedRegion() const noexcept;
	ND inline D2D1_RECT_F GetMaxTextAllowedRegion() const noexcept;
	ND inline D2D1_RECT_F GetValueTextOnRightAllowedRegion() const noexcept;
	ND inline D2D1_RECT_F GetPopUpRect() const noexcept;

	void UpdateValueTexts();

	float m_minValue;
	float m_maxValue;
	float m_value;

	float m_lineLeftX;
	float m_lineRightX;
	float m_lineY;
	float m_lineWidth;

	float m_circlePositionX;
	float m_circleRadius;
	float m_circleRadius2;

	bool m_fillLineRight; // If false, it will just outline the right side

	std::unique_ptr<ColorBrush> m_lineBrushLeft;
	std::unique_ptr<ColorBrush> m_lineBrushRight;
	std::unique_ptr<ColorBrush> m_circleBrush;
	std::unique_ptr<ColorBrush> m_circleBrush2;

	MouseOverCircleState m_mouseOverCircleState;

	std::unique_ptr<Text> m_minText;
	float m_minTextXOffset;
	float m_minTextYOffset;
	std::unique_ptr<Text> m_maxText;
	float m_maxTextXOffset;
	float m_maxTextYOffset;
	bool m_showMinMaxTextValues;

	// Value format
	std::wstring m_valueFormatString;

	// Value TextInput On Right
	std::unique_ptr<TextInput> m_valueTextInputOnRight;
	bool m_showValueRightOfSlider;
	float m_marginRightOfSlider;
	float m_valueTextInputOnRightWidth;
	float m_valueTextInputOnRightHeight;

	// Value Text On Pop Up
	std::unique_ptr<Text> m_valueTextOnPopUp;
	bool m_showValueAsPopUpWhenSliding;
	std::unique_ptr<ColorBrush> m_popUpBackgroundBrush;
	std::unique_ptr<ColorBrush> m_popUpBorderBrush;
	float m_popUpBorderWidth;
	float m_popUpCornerRadiusX;
	float m_popUpCornerRadiusY;
	float m_popUpHeight;
	float m_popUpWidth;
};
#pragma warning( pop )
}