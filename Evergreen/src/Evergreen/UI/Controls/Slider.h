#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/UI/Controls/Text.h"
#include "Evergreen/UI/Controls/TextInput.h"
#include "Evergreen/UI/Controls/Slider.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
template<typename T>
class Slider : public Control
{
public:
	Slider(std::shared_ptr<DeviceResources> deviceResources,
		UI* ui,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		T minValue = 0, T maxValue = 1, T initialValue = 0,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	Slider(const Slider&) noexcept = delete; // Just delete for now until there is a good use case
	void operator=(const Slider&) noexcept = delete;
	virtual ~Slider() noexcept override {}

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

	void SetValueFormatString(const std::wstring& fmt) noexcept { m_valueFormatString = fmt; }

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
	ND T PixelToValue(float pixel) const noexcept;

	ND inline bool CircleContainsPoint(float x, float y) const noexcept;

	ND inline D2D1_RECT_F GetMinTextAllowedRegion() const noexcept;
	ND inline D2D1_RECT_F GetMaxTextAllowedRegion() const noexcept;
	ND inline D2D1_RECT_F GetValueTextOnRightAllowedRegion() const noexcept;
	ND inline D2D1_RECT_F GetPopUpRect() const noexcept;

	void UpdateValueTexts();

	T m_minValue;
	T m_maxValue;
	T m_value;

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
	float m_valueTextOnRightAllowedWidth;

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

template<typename T>
Slider<T>::Slider(std::shared_ptr<DeviceResources> deviceResources,
				UI* ui,
				const D2D1_RECT_F& allowedRegion,
				T minValue, T maxValue, T initialValue,
				const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, ui, allowedRegion, margin),
	m_minValue(minValue),
	m_maxValue(maxValue),
	m_value(initialValue),
	m_lineWidth(5.0f),
	m_circleRadius(5.0f),
	m_circleRadius2(10.0f),
	m_fillLineRight(false),
	m_lineBrushLeft(nullptr),
	m_lineBrushRight(nullptr),
	m_circleBrush(nullptr),
	m_circleBrush2(nullptr),
	m_mouseOverCircleState(MouseOverCircleState::NOT_OVER),
	m_minText(nullptr),
	m_minTextXOffset(5.0f),
	m_minTextYOffset(5.0f),
	m_maxText(nullptr),
	m_maxTextXOffset(5.0f),
	m_maxTextYOffset(5.0f),
	m_showMinMaxTextValues(true),
	m_valueFormatString(L"{:.2f}"),
	m_valueTextInputOnRight(nullptr),
	m_showValueRightOfSlider(true),
	m_valueTextOnRightAllowedWidth(50.0f),
	m_valueTextOnPopUp(nullptr),
	m_showValueAsPopUpWhenSliding(true),
	m_popUpBackgroundBrush(nullptr),
	m_popUpBorderBrush(nullptr),
	m_popUpBorderWidth(1.0f),
	m_popUpCornerRadiusX(5.0f),
	m_popUpCornerRadiusY(5.0f),
	m_popUpHeight(35.0f),
	m_popUpWidth(35.0f)
{
	if (m_lineBrushLeft == nullptr)
		m_lineBrushLeft = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue));

	if (m_lineBrushRight == nullptr)
		m_lineBrushRight = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue));

	if (m_circleBrush == nullptr)
		m_circleBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue));

	if (m_circleBrush2 == nullptr)
		m_circleBrush2 = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue, 0.25f));

	if (m_popUpBackgroundBrush == nullptr)
		m_popUpBackgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::LightBlue));

	if (m_popUpBorderBrush == nullptr)
		m_popUpBorderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::DarkBlue));


	SliderChanged();

	// Min Text
	std::unique_ptr<TextStyle> minValueTextStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"Slider Minimum Value Text Style",
		Evergreen::FontFamily::Calibri,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
	);
	m_minText = std::make_unique<Text>(
		m_deviceResources,
		m_ui,
		GetMinTextAllowedRegion(),
		std::format(L"{}", m_minValue),
		nullptr,
		std::move(minValueTextStyle)
	);

	// Max Text
	std::unique_ptr<TextStyle> maxValueTextStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"Slider Maximum Value Text Style",
		Evergreen::FontFamily::Calibri,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
	);
	m_maxText = std::make_unique<Text>(
		m_deviceResources,
		m_ui,
		GetMaxTextAllowedRegion(),
		std::format(L"{}", m_maxValue),
		nullptr,
		std::move(maxValueTextStyle)
	);

	// Value Text On Right
	std::unique_ptr<TextStyle> valueTextInputOnRightTextStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"Slider Value TextInput On Right Text Style",
		Evergreen::FontFamily::Calibri,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
	);
	m_valueTextInputOnRight = std::make_unique<TextInput>(
		m_deviceResources,
		m_ui,
		GetValueTextOnRightAllowedRegion(),
		L"", // placeholder text
		nullptr, // placeholder brush
		nullptr, // placeholder style
		nullptr, // input text brush
		std::move(valueTextInputOnRightTextStyle), // input text style
		std::move(std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f))), // background brush
		nullptr, // border brush
		0.0f // border width
	);
	m_valueTextInputOnRight->SetInputText(std::format(L"{}", m_value));



	// Value Text On Pop Up
	std::unique_ptr<TextStyle> valueTextOnPopUpTextStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"Slider Value Text On Pop Up Text Style",
		Evergreen::FontFamily::Calibri,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
	);
	m_valueTextOnPopUp = std::make_unique<Text>(
		m_deviceResources,
		m_ui,
		GetPopUpRect(),
		std::format(L"{}", m_value),
		nullptr,
		std::move(valueTextOnPopUpTextStyle)
	);
}

template<typename T>
D2D1_RECT_F Slider<T>::GetMinTextAllowedRegion() const noexcept
{
	return D2D1::RectF(m_lineLeftX + m_minTextXOffset, m_lineY + m_minTextYOffset, m_allowedRegion.right, m_allowedRegion.bottom);
}
template<typename T>
D2D1_RECT_F Slider<T>::GetMaxTextAllowedRegion() const noexcept
{
	return D2D1::RectF(m_allowedRegion.left, m_lineY + m_maxTextYOffset, m_lineRightX - m_maxTextXOffset, m_allowedRegion.bottom);
}
template<typename T>
D2D1_RECT_F Slider<T>::GetValueTextOnRightAllowedRegion() const noexcept
{
	return D2D1::RectF(m_allowedRegion.right - m_valueTextOnRightAllowedWidth, m_allowedRegion.top, m_allowedRegion.right, m_allowedRegion.bottom);
}
template<typename T>
D2D1_RECT_F Slider<T>::GetPopUpRect() const noexcept
{
	float halfWidth = m_popUpWidth / 2;

	return D2D1::RectF(
		m_circlePositionX - halfWidth,
		m_lineY - m_popUpHeight - m_circleRadius,
		m_circlePositionX + halfWidth,
		m_lineY - m_circleRadius
	);
}


template<typename T>
void Slider<T>::UpdateValueTexts()
{
	// I guess you need to use std::vformat in order to pass a variable as the format parameter...
	std::wstring str = std::vformat(m_valueFormatString, std::make_wformat_args(m_value));
	m_valueTextInputOnRight->SetInputText(str);
	m_valueTextOnPopUp->SetText(str);
}

template<typename T>
void Slider<T>::SliderChanged()
{
	m_lineLeftX = m_allowedRegion.left + m_margin.Left;
	m_lineRightX = m_showValueRightOfSlider ? m_allowedRegion.right - m_margin.Right - m_valueTextOnRightAllowedWidth : m_allowedRegion.right - m_margin.Right;
	m_lineY = m_allowedRegion.top + ((m_allowedRegion.bottom - m_allowedRegion.top) / 2);
	m_circlePositionX = ValueToPixel();
}
template<typename T>
void Slider<T>::OnMarginChanged()
{
	SliderChanged();
}
template<typename T>
void Slider<T>::OnAllowedRegionChanged()
{
	EG_CORE_ASSERT(m_lineBrushLeft != nullptr, "No line brush left");
	EG_CORE_ASSERT(m_lineBrushRight != nullptr, "No line brush right");
	EG_CORE_ASSERT(m_circleBrush != nullptr, "No circle brush");

	// If using a non-SolidColorBrush, we need to update the draw region for the brushes
	m_lineBrushLeft->SetDrawRegion(m_allowedRegion);
	m_lineBrushRight->SetDrawRegion(m_allowedRegion);
	m_circleBrush->SetDrawRegion(m_allowedRegion);

	// Update Text locations
	m_minText->AllowedRegion(GetMinTextAllowedRegion());
	m_maxText->AllowedRegion(GetMaxTextAllowedRegion());
	m_valueTextInputOnRight->AllowedRegion(GetValueTextOnRightAllowedRegion());
	m_valueTextOnPopUp->AllowedRegion(GetPopUpRect());

	SliderChanged();
}
template<typename T>
float Slider<T>::ValueToPixel() const noexcept
{
	if (m_value <= m_minValue)
		return m_lineLeftX;

	if (m_value >= m_maxValue)
		return m_lineRightX;

	float pixelPerValue = (m_lineRightX - m_lineLeftX) / static_cast<float>(m_maxValue - m_minValue);
	return static_cast<float>(m_value - m_minValue) * pixelPerValue + m_lineLeftX;
}
template<typename T>
T Slider<T>::PixelToValue(float pixel) const noexcept
{
	if (pixel < m_lineLeftX)
		return m_minValue;

	if (pixel > m_lineRightX)
		return m_maxValue;

	float valuePerPixel = static_cast<float>(m_maxValue - m_minValue) / (m_lineRightX - m_lineLeftX);
	return static_cast<T>(valuePerPixel * (pixel - m_lineLeftX) + m_minValue);
}

template<typename T>
void Slider<T>::Render() const noexcept
{
	//EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	auto context = m_deviceResources->D2DDeviceContext();

	float halfLineWidth = m_lineWidth / 2;
	float lineTop = m_lineY - halfLineWidth;
	float lineBottom = m_lineY + halfLineWidth;

	// Left Line
	if (m_circlePositionX > m_lineLeftX)
	{
		D2D1_RECT_F leftRect = D2D1::RectF(m_lineLeftX, lineTop, m_circlePositionX, lineBottom);
		D2D1_ROUNDED_RECT leftRoundedRect = D2D1::RoundedRect(leftRect, halfLineWidth, halfLineWidth);
		context->FillRoundedRectangle(leftRoundedRect, m_lineBrushLeft->Get());
	}

	// Right Line
	if (m_circlePositionX < m_lineRightX)
	{
		D2D1_RECT_F rightRect = D2D1::RectF(m_circlePositionX, lineTop, m_lineRightX, lineBottom);
		D2D1_ROUNDED_RECT rightRoundedRect = D2D1::RoundedRect(rightRect, halfLineWidth, halfLineWidth);

		if (m_fillLineRight)
			context->FillRoundedRectangle(rightRoundedRect, m_lineBrushRight->Get());
		else
			context->DrawRoundedRectangle(rightRoundedRect, m_lineBrushRight->Get(), 1.0f);
	}

	// Circle
	D2D1_ELLIPSE circle = D2D1::Ellipse(D2D1::Point2F(m_circlePositionX, m_lineY), m_circleRadius, m_circleRadius);
	context->FillEllipse(circle, m_circleBrush->Get());

	// Circle 2
	if (m_mouseOverCircleState != MouseOverCircleState::NOT_OVER && m_circleBrush2 != nullptr)
	{
		D2D1_ELLIPSE circle2 = D2D1::Ellipse(D2D1::Point2F(m_circlePositionX, m_lineY), m_circleRadius2, m_circleRadius2);
		context->FillEllipse(circle2, m_circleBrush2->Get());
	}

	// Min/Max text
	if (m_showMinMaxTextValues)
	{
		m_minText->Render();
		m_maxText->Render();
	}

	if (m_showValueRightOfSlider)
		m_valueTextInputOnRight->Render();

	if (m_showValueAsPopUpWhenSliding && m_mouseOverCircleState == MouseOverCircleState::DRAGGING)
	{
		D2D1_RECT_F rect = GetPopUpRect();
		if (m_popUpCornerRadiusX > 0.0f && m_popUpCornerRadiusY > 0.0f)
		{
			D2D1_ROUNDED_RECT popUpRounded = D2D1::RoundedRect(rect, m_popUpCornerRadiusX, m_popUpCornerRadiusY);
			context->FillRoundedRectangle(popUpRounded, m_popUpBackgroundBrush->Get());
			if (m_popUpBorderWidth > 0.0f)
				context->DrawRoundedRectangle(popUpRounded, m_popUpBorderBrush->Get(), m_popUpBorderWidth);
		}
		else
		{
			context->FillRectangle(rect, m_popUpBackgroundBrush->Get());
			if (m_popUpBorderWidth > 0.0f)
				context->DrawRectangle(rect, m_popUpBorderBrush->Get(), m_popUpBorderWidth);
		}
		

		m_valueTextOnPopUp->Render();
	}
}

template<typename T>
void Slider<T>::OnMouseMove(MouseMoveEvent& e) noexcept
{
	switch (m_mouseOverCircleState)
	{
	case MouseOverCircleState::DRAGGING:
	{
		if (e.GetX() > m_lineRightX)
		{
			m_circlePositionX = m_lineRightX;

			if (m_value != m_maxValue)
			{
				m_value = m_maxValue;
				UpdateValueTexts();
				// TRIGGER OnValueChanged() +++++++++++++++++++++++++++++++
			}
		}
		else if (e.GetX() < m_lineLeftX)
		{
			m_circlePositionX = m_lineLeftX;
			if (m_value != m_minValue)
			{
				m_value = m_minValue;
				UpdateValueTexts();
				// TRIGGER OnValueChanged() +++++++++++++++++++++++++++++++
			}
		}
		else
		{
			m_circlePositionX = e.GetX();
			T newValue = PixelToValue(m_circlePositionX);
			if (newValue != m_value)
			{
				m_value = newValue;
				UpdateValueTexts();
				// TRIGGER OnValueChanged() +++++++++++++++++++++++++++++++
			}
		}

		m_valueTextOnPopUp->AllowedRegion(GetPopUpRect());
		e.Handled(this);
		return;
	}
	case MouseOverCircleState::NOT_OVER:
	{
		if (CircleContainsPoint(e.GetX(), e.GetY()))
		{
			m_mouseOverCircleState = MouseOverCircleState::OVER;
			// TRIGGER OnMouseEnteredCircle() +++++++++++++++++++++++++++++++
			e.Handled(this);
			return;
		}

		break;
	}
	case MouseOverCircleState::OVER:
	{
		if (!CircleContainsPoint(e.GetX(), e.GetY()))
		{
			m_mouseOverCircleState = MouseOverCircleState::NOT_OVER;
			// TRIGGER OnMouseExitedCircle() +++++++++++++++++++++++++++++++
			break;
		}
		e.Handled(this);
		return;
	}
	}

	m_valueTextInputOnRight->OnMouseMove(e);
}
template<typename T>
void Slider<T>::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	if (m_mouseOverCircleState == MouseOverCircleState::OVER && e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		m_mouseOverCircleState = MouseOverCircleState::DRAGGING;
		// TRIGGER OnMouseBeginDragging() +++++++++++++++++++++++++++++++
		e.Handled(this);
		return;
	}

	m_valueTextInputOnRight->OnMouseButtonPressed(e);
}
template<typename T>
void Slider<T>::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	if (m_mouseOverCircleState == MouseOverCircleState::DRAGGING && e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		m_mouseOverCircleState = CircleContainsPoint(e.GetX(), e.GetY()) ? MouseOverCircleState::OVER : MouseOverCircleState::NOT_OVER;
		// TRIGGER OnMouseStoppedDragging() +++++++++++++++++++++++++++++++
		e.Handled(this);
		return;
	}

	m_valueTextInputOnRight->OnMouseButtonReleased(e);
}

template<typename T>
bool Slider<T>::CircleContainsPoint(float x, float y) const noexcept
{
	float delX = m_circlePositionX - x;
	float delY = m_lineY - y;
	float distance = std::sqrtf((delX * delX) + (delY * delY));
	return distance <= m_circleRadius;
}

}
