#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/UI/Controls/Text.h"
#include "Evergreen/UI/Controls/TextInput.h"

namespace Evergreen
{
class EVERGREEN_API SliderFloatValueChangedEvent : public Event
{
public:
	SliderFloatValueChangedEvent(float value) noexcept : m_value(value)
	{}
	SliderFloatValueChangedEvent(const SliderFloatValueChangedEvent&) = delete;
	void operator=(const SliderFloatValueChangedEvent&) = delete;

	inline float GetValue() const noexcept { return m_value; }

	std::string ToString() const noexcept override { return std::format("SliderFloatValueChangedEvent: {}", m_value); }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategory::None; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::None; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "SliderFloatValueChanged"; }

private:
	float m_value;
};


// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API SliderFloat : public Control
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
	void Render() const override;

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

	ND inline float GetMinimumValue() const noexcept { return m_minValue; }
	ND inline float GetMaximumValue() const noexcept { return m_maxValue; }
	ND inline float GetValue() const noexcept { return m_value; }
	ND inline float GetLineWidth() const noexcept { return m_lineWidth; }
	ND inline float GetCircleRadius() const noexcept { return m_circleRadius; }
	ND inline float GetCircleRadiusOuter() const noexcept { return m_circleRadius2; }
	ND inline bool GetFillLineOnRightSide() const noexcept { return m_fillLineRight; }
	ND inline ColorBrush* GetLineBrushLeft() const noexcept { return m_lineBrushLeft.get(); }
	ND inline ColorBrush* GetLineBrushRight() const noexcept { return m_lineBrushRight.get(); }
	ND inline ColorBrush* GetCircleBrush() const noexcept { return m_circleBrush.get(); }
	ND inline ColorBrush* GetCircleBrushOuter() const noexcept { return m_circleBrush2.get(); }
	ND inline float GetMinTextXOffset() const noexcept { return m_minTextXOffset; }
	ND inline float GetMinTextYOffset() const noexcept { return m_minTextYOffset; }
	ND inline float GetMaxTextXOffset() const noexcept { return m_maxTextXOffset; }
	ND inline float GetMaxTextYOffset() const noexcept { return m_maxTextYOffset; }
	ND inline bool GetShowMinMaxTextValues() const noexcept { return m_showMinMaxTextValues; }
	ND inline bool GetShowValueRightOfSlider() const noexcept { return m_showValueRightOfSlider; }
	ND inline float GetMarginRightOfSlider() const noexcept { return m_marginRightOfSlider; }
	ND inline float GetTextInputHeight() const noexcept { return m_valueTextInputOnRightHeight; }
	ND inline float GetTextInputWidth() const noexcept { return m_valueTextInputOnRightWidth; }
	ND inline TextInput* GetTextInput() const noexcept { return m_valueTextInputOnRight.get(); }
	ND inline bool GetShowValueAsPopUpWhenSliding() const noexcept { return m_showValueAsPopUpWhenSliding; }
	ND inline ColorBrush* GetPopUpBackgroundBrush() const noexcept { return m_popUpBackgroundBrush.get(); }
	ND inline ColorBrush* GetPopUpBorderBrush() const noexcept { return m_popUpBorderBrush.get(); }
	ND inline float GetPopUpBorderWidth() const noexcept { return m_popUpBorderWidth; }
	ND inline float GetPopUpCornerRadiusX() const noexcept{ return m_popUpCornerRadiusX; }
	ND inline float GetPopUpCornerRadiusY() const noexcept{ return m_popUpCornerRadiusY; }
	ND inline float GetPopUpHeight() const noexcept { return m_popUpHeight; }
	ND inline float GetPopUpWidth() const noexcept { return m_popUpWidth; }
	ND inline bool IsSliderDragging() const noexcept { return m_mouseOverCircleState == MouseOverCircleState::DRAGGING; }
	ND inline TextStyle* GetMinTextStyle() const noexcept { return m_minText->GetTextStyle(); }
	ND inline TextStyle* GetMaxTextStyle() const noexcept { return m_maxText->GetTextStyle(); }
	ND inline TextStyle* GetTextInputStyle() const noexcept { return m_valueTextInputOnRight->GetInputTextStyle(); }
	ND inline TextStyle* GetPopUpTextStyle() const noexcept { return m_valueTextOnPopUp->GetTextStyle(); }

	void SetMinimumValue(float minimum) noexcept;
	void SetMaximumValue(float maximum) noexcept;
	void SetMiniumAndMaximumValues(float minimum, float maximum) noexcept;
	void SetValue(float value) noexcept;
	inline void SetLineWidth(float width) noexcept { m_lineWidth = width; }
	inline void SetCircleRadius(float radius) noexcept { m_circleRadius = radius; m_valueTextOnPopUp->AllowedRegion(GetPopUpRect()); }
	inline void SetCircleRadiusOuter(float radius) noexcept { m_circleRadius2 = radius; }
	inline void SetFillLineOnRightSide(bool fill) noexcept { m_fillLineRight = fill; }
	void SetLineBrushLeft(std::unique_ptr<ColorBrush> brush) noexcept;
	void SetLineBrushRight(std::unique_ptr<ColorBrush> brush) noexcept;
	void SetCircleBrush(std::unique_ptr<ColorBrush> brush) noexcept;
	void SetCircleBrushOuter(std::unique_ptr<ColorBrush> brush) noexcept;
	inline void SetMinTextXOffset(float offset) noexcept { m_minTextXOffset = offset; m_minText->AllowedRegion(GetMinTextAllowedRegion()); }
	inline void SetMinTextYOffset(float offset) noexcept { m_minTextYOffset = offset; m_minText->AllowedRegion(GetMinTextAllowedRegion()); }
	inline void SetMaxTextXOffset(float offset) noexcept { m_maxTextXOffset = offset; m_maxText->AllowedRegion(GetMaxTextAllowedRegion()); }
	inline void SetMaxTextYOffset(float offset) noexcept { m_maxTextYOffset = offset; m_maxText->AllowedRegion(GetMaxTextAllowedRegion()); }
	inline void SetShowMinMaxTextValues(bool show) noexcept { m_showMinMaxTextValues = show; }
	inline void SetShowValueRightOfSlider(bool show) noexcept { m_showValueRightOfSlider = show; SliderChanged(); }
	inline void SetMarginRightOfSlider(float margin) noexcept { m_marginRightOfSlider = margin; SliderChanged(); }
	void SetTextInputHeight(float height) noexcept;
	void SetTextInputWidth(float width) noexcept;
	void SetTextInputHeightAndWidth(float height, float width) noexcept;	
	inline void SetTextInputBackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_valueTextInputOnRight->SetBackgroundBrush(std::move(brush)); }
	inline void SetTextInputBorderBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_valueTextInputOnRight->SetBorderBrush(std::move(brush)); }
	inline void SetTextInputBorderWidth(float width) noexcept { m_valueTextInputOnRight->SetBorderWidth(width); }
	inline void SetShowValueAsPopUpWhenSliding(bool show) noexcept { m_showValueAsPopUpWhenSliding = show; }
	void SetPopUpBackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept;
	void SetPopUpBorderBrush(std::unique_ptr<ColorBrush> brush) noexcept;
	inline void SetPopUpBorderWidth(float width) noexcept { m_popUpBorderWidth = width; }
	inline void SetPopUpCornerRadius(float radius) noexcept { m_popUpCornerRadiusX = radius; m_popUpCornerRadiusY = radius; }
	inline void SetPopUpCornerRadius(float radiusX, float radiusY) noexcept { m_popUpCornerRadiusX = radiusX; m_popUpCornerRadiusY = radiusY; }
	inline void SetPopUpCornerRadiusX(float radius) noexcept { m_popUpCornerRadiusX = radius; }
	inline void SetPopUpCornerRadiusY(float radius) noexcept { m_popUpCornerRadiusY = radius; }
	inline void SetPopUpHeight(float height) noexcept { m_popUpHeight = height; SliderChanged(); }
	inline void SetPopUpWidth(float width) noexcept { m_popUpWidth = width; SliderChanged(); }
	inline void SetMinTextBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_minText->SetColorBrush(std::move(brush)); }
	inline void SetMinTextStyle(std::unique_ptr<TextStyle> style) noexcept { m_minText->SetTextStyle(std::move(style)); }
	inline void SetMaxTextBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_maxText->SetColorBrush(std::move(brush)); }
	inline void SetMaxTextStyle(std::unique_ptr<TextStyle> style) noexcept { m_maxText->SetTextStyle(std::move(style)); }
	inline void SetTextInputTextBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_valueTextInputOnRight->SetInputTextBrush(std::move(brush)); }
	inline void SetTextInputTextStyle(std::unique_ptr<TextStyle> style) noexcept { m_valueTextInputOnRight->SetInputTextStyle(std::move(style)); }
	inline void SetPopUpTextBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_valueTextOnPopUp->SetColorBrush(std::move(brush)); }
	inline void SetPopUpTextStyle(std::unique_ptr<TextStyle> style) noexcept { m_valueTextOnPopUp->SetTextStyle(std::move(style)); }

	inline void SetValueFormatString(const std::wstring& fmt) noexcept { m_valueFormatString = fmt; UpdateValueTexts();  }
	
	inline void SetOnMouseEnteredCircleCallback(std::function<void(SliderFloat*, MouseMoveEvent& e)> func) noexcept { m_OnMouseEnteredCircle = func; }
	inline void SetOnMouseExitedCircleCallback(std::function<void(SliderFloat*, MouseMoveEvent& e)> func) noexcept { m_OnMouseExitedCircle = func; }
	inline void SetOnBeginDraggingCallback(std::function<void(SliderFloat*, MouseButtonPressedEvent& e)> func) noexcept { m_OnBeginDragging = func; }
	inline void SetOnStoppedDraggingCallback(std::function<void(SliderFloat*, MouseButtonReleasedEvent& e)> func) noexcept { m_OnStoppedDragging = func; }
	inline void SetOnValueChangedCallback(std::function<void(SliderFloat*, SliderFloatValueChangedEvent& e)> func) noexcept { m_OnValueChanged = func; }

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

	std::function<void(SliderFloat*, MouseMoveEvent&)> m_OnMouseEnteredCircle = [](SliderFloat*, MouseMoveEvent&) {};
	std::function<void(SliderFloat*, MouseMoveEvent&)> m_OnMouseExitedCircle = [](SliderFloat*, MouseMoveEvent&) {};
	std::function<void(SliderFloat*, MouseButtonPressedEvent&)> m_OnBeginDragging = [](SliderFloat*, MouseButtonPressedEvent&) {};
	std::function<void(SliderFloat*, MouseButtonReleasedEvent&)> m_OnStoppedDragging = [](SliderFloat*, MouseButtonReleasedEvent&) {};
	std::function<void(SliderFloat*, SliderFloatValueChangedEvent&)> m_OnValueChanged = [](SliderFloat*, SliderFloatValueChangedEvent&) {};

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
	bool m_mouseIsOverCircle; // Necessary to have this so we can track this separately when dragging

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