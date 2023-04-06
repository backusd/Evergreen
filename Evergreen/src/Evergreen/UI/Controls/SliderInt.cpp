#include "pch.h"
#include "SliderInt.h"

namespace Evergreen
{
SliderInt::SliderInt(std::shared_ptr<DeviceResources> deviceResources,
	UI* ui,
	const D2D1_RECT_F& allowedRegion,
	int minValue, int maxValue, int initialValue,
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
	m_mouseIsOverCircle(false),
	m_minText(nullptr),
	m_minTextXOffset(5.0f),
	m_minTextYOffset(5.0f),
	m_maxText(nullptr),
	m_maxTextXOffset(5.0f),
	m_maxTextYOffset(5.0f),
	m_showMinMaxTextValues(true),
	m_valueFormatString(L"{}"),
	m_valueTextInputOnRight(nullptr),
	m_showValueRightOfSlider(true),
	m_marginRightOfSlider(50.0f),
	m_valueTextInputOnRightWidth(40.0f),
	m_valueTextInputOnRightHeight(25.0f),
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


	m_lineLeftX = m_allowedRegion.left + m_margin.Left;
	m_lineRightX = m_showValueRightOfSlider ? m_allowedRegion.right - m_margin.Right - m_marginRightOfSlider : m_allowedRegion.right - m_margin.Right;
	EG_CORE_ASSERT(m_lineLeftX < m_lineRightX, "Margin might be too large.");
	m_lineY = m_allowedRegion.top + ((m_allowedRegion.bottom - m_allowedRegion.top) / 2);
	m_circlePositionX = ValueToPixel();

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
		std::move(std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::LightGray))), // background brush
		std::move(std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray))), // border brush
		1.0f // border width
		);
	m_valueTextInputOnRight->SetInputText(std::format(L"{}", m_value));
	m_valueTextInputOnRight->SetOnEnterKeyCallback(
		[this](Control* control, Event& e)
		{
			EG_CORE_ASSERT(this != nullptr, "Something went wrong!!");
			EG_CORE_ASSERT(control != nullptr, "Something went wrong!!");

			TextInput* ti = static_cast<TextInput*>(control);
			std::wstring text = ti->GetInputText();
			if (text.size() == 0)
			{
				this->UpdateValueTexts(); // Just reset the text to whatever it was previously
				return;
			}

			try
			{
				int value = std::stoi(text);
				if (value < this->GetMinimumValue())
				{
					EG_CORE_WARN("{}:{} - SliderInt with name '{}': Ignoring input value ({}) because it is less than minimum allowed value ({})", __FILE__, __LINE__, this->Name(), value, this->GetMinimumValue());
					this->UpdateValueTexts(); // Just reset the text to whatever it was previously
				}
				else if (value > this->GetMaximumValue())
				{
					EG_CORE_WARN("{}:{} - SliderInt with name '{}': Ignoring input value ({}) because it is greater than maximum allowed value ({})", __FILE__, __LINE__, this->Name(), value, this->GetMaximumValue());
					this->UpdateValueTexts(); // Just reset the text to whatever it was previously
				}
				else
				{
					this->SetValue(value);
				}
			}
			catch (const std::invalid_argument& ex) {
				EG_CORE_WARN("{}:{} - SliderInt with name '{}': caught std::invalid_argument exception with message: {}", __FILE__, __LINE__, this->Name(), ex.what());
				this->UpdateValueTexts(); // Just reset the text to whatever it was previously
			}
			catch (const std::out_of_range& ex) {
				EG_CORE_WARN("{}:{} - SliderInt with name '{}': caught std::out_of_range exception with message: {}", __FILE__, __LINE__, this->Name(), ex.what());
				this->UpdateValueTexts(); // Just reset the text to whatever it was previously
			}
		}
	);


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


D2D1_RECT_F SliderInt::GetMinTextAllowedRegion() const noexcept
{
	return D2D1::RectF(m_lineLeftX + m_minTextXOffset, m_lineY + m_minTextYOffset, m_allowedRegion.right, m_allowedRegion.bottom);
}
D2D1_RECT_F SliderInt::GetMaxTextAllowedRegion() const noexcept
{
	return D2D1::RectF(m_allowedRegion.left, m_lineY + m_maxTextYOffset, m_lineRightX - m_maxTextXOffset, m_allowedRegion.bottom);
}
D2D1_RECT_F SliderInt::GetValueTextOnRightAllowedRegion() const noexcept
{
	float halfHeight = m_valueTextInputOnRightHeight / 2;
	float left = m_lineRightX + 5.0f;
	return D2D1::RectF(
		left,
		m_lineY - halfHeight,
		left + m_valueTextInputOnRightWidth,
		m_lineY + halfHeight
	);
}
D2D1_RECT_F SliderInt::GetPopUpRect() const noexcept
{
	float halfWidth = m_popUpWidth / 2;

	return D2D1::RectF(
		m_circlePositionX - halfWidth,
		m_lineY - m_popUpHeight - m_circleRadius,
		m_circlePositionX + halfWidth,
		m_lineY - m_circleRadius
	);
}



void SliderInt::UpdateValueTexts()
{
	EG_CORE_ASSERT(m_valueTextInputOnRight != nullptr, "Should not be nullptr");
	EG_CORE_ASSERT(m_valueTextOnPopUp != nullptr, "Should not be nullptr");

	// I guess you need to use std::vformat in order to pass a variable as the format parameter...
	std::wstring str = std::vformat(m_valueFormatString, std::make_wformat_args(m_value));
	m_valueTextInputOnRight->SetInputText(str);
	m_valueTextOnPopUp->SetText(str);
}
void SliderInt::SliderChanged()
{
	EG_CORE_ASSERT(m_minText != nullptr, "Should not be nullptr");
	EG_CORE_ASSERT(m_maxText != nullptr, "Should not be nullptr");
	EG_CORE_ASSERT(m_valueTextInputOnRight != nullptr, "Should not be nullptr");
	EG_CORE_ASSERT(m_valueTextOnPopUp != nullptr, "Should not be nullptr");

	m_lineLeftX = m_allowedRegion.left + m_margin.Left;
	m_lineRightX = m_showValueRightOfSlider ? m_allowedRegion.right - m_margin.Right - m_marginRightOfSlider : m_allowedRegion.right - m_margin.Right;
	m_lineY = m_allowedRegion.top + ((m_allowedRegion.bottom - m_allowedRegion.top) / 2);
	m_circlePositionX = ValueToPixel();

	// Update Text locations
	m_minText->AllowedRegion(GetMinTextAllowedRegion());
	m_maxText->AllowedRegion(GetMaxTextAllowedRegion());
	m_valueTextInputOnRight->AllowedRegion(GetValueTextOnRightAllowedRegion());
	m_valueTextOnPopUp->AllowedRegion(GetPopUpRect());

	UpdateValueTexts();
}

void SliderInt::OnMarginChanged()
{
	SliderChanged();
}
void SliderInt::OnAllowedRegionChanged()
{
	EG_CORE_ASSERT(m_lineBrushLeft != nullptr, "No line brush left");
	EG_CORE_ASSERT(m_lineBrushRight != nullptr, "No line brush right");
	EG_CORE_ASSERT(m_circleBrush != nullptr, "No circle brush");
	EG_CORE_ASSERT(m_circleBrush2 != nullptr, "No circle brush");

	// If using a non-SolidColorBrush, we need to update the draw region for the brushes
	m_lineBrushLeft->SetDrawRegion(m_allowedRegion);
	m_lineBrushRight->SetDrawRegion(m_allowedRegion);
	m_circleBrush->SetDrawRegion(m_allowedRegion);
	m_circleBrush2->SetDrawRegion(m_allowedRegion);

	SliderChanged();
}

float SliderInt::ValueToPixel() const noexcept
{
	if (m_value <= m_minValue)
		return m_lineLeftX;

	if (m_value >= m_maxValue)
		return m_lineRightX;

	float pixelPerValue = (m_lineRightX - m_lineLeftX) / static_cast<float>(m_maxValue - m_minValue);
	return static_cast<float>(m_value - m_minValue) * pixelPerValue + m_lineLeftX;
}
int SliderInt::PixelToValue(float pixel) const noexcept
{
	if (pixel < m_lineLeftX)
		return m_minValue;

	if (pixel > m_lineRightX)
		return m_maxValue;

	float valuePerPixel = static_cast<float>(m_maxValue - m_minValue) / (m_lineRightX - m_lineLeftX);
	return static_cast<int>(valuePerPixel * (pixel - m_lineLeftX)) + m_minValue;
}


void SliderInt::Render() const
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_lineBrushLeft != nullptr, "No line brush left");
	EG_CORE_ASSERT(m_lineBrushRight != nullptr, "No line brush right");
	EG_CORE_ASSERT(m_circleBrush != nullptr, "No circle brush");
	EG_CORE_ASSERT(m_circleBrush2 != nullptr, "No circle brush");
	EG_CORE_ASSERT(m_minText != nullptr, "Should not be nullptr");
	EG_CORE_ASSERT(m_maxText != nullptr, "Should not be nullptr");
	EG_CORE_ASSERT(m_valueTextInputOnRight != nullptr, "Should not be nullptr");
	EG_CORE_ASSERT(m_valueTextOnPopUp != nullptr, "Should not be nullptr");

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




void SliderInt::SetMinimumValue(int minimum) noexcept
{
	m_minValue = minimum;

	if (m_minValue >= m_maxValue)
	{
		// Only register a warning here because the user may call SetMaximumValue after this and set it to an appropriate value
		EG_CORE_WARN("{}:{} - SliderInt: New minimum value ({}) is greater than or equal to current maximum value ({}).", __FILE__, __LINE__, minimum, m_maxValue);
		return;
	}

	if (m_value < minimum)
	{
		EG_CORE_WARN("{}:{} - SliderInt: Current value ({}) is less than the new minimum value ({}). Setting current value to new minimum.", __FILE__, __LINE__, m_value, minimum);
		m_value = minimum;
	}

	SliderChanged();
}
void SliderInt::SetMaximumValue(int maximum) noexcept
{
	m_maxValue = maximum;

	if (maximum <= m_minValue)
	{
		// Only register a warning here because the user may call SetMinimumValue after this and set it to an appropriate value
		EG_CORE_WARN("{}:{} - SliderInt: New maximum value ({}) is less than or equal to current minimum value ({}).", __FILE__, __LINE__, maximum, m_minValue);
		return;
	}

	if (m_value > maximum)
	{
		EG_CORE_WARN("{}:{} - SliderInt: Current value ({}) is greater than the new maximum value ({}). Setting current value to new maximum.", __FILE__, __LINE__, m_value, maximum);
		m_value = maximum;
	}

	SliderChanged();
}
void SliderInt::SetMiniumAndMaximumValues(int minimum, int maximum) noexcept
{
	m_minValue = minimum;
	m_maxValue = maximum;

	if (minimum > maximum)
	{
		EG_CORE_ERROR("{}:{} - SliderInt: New maximum value ({}) must be greater than new minimum value ({}). IGNORING this change.", __FILE__, __LINE__, maximum, minimum);
		return;
	}

	if (m_value < minimum)
	{
		EG_CORE_WARN("{}:{} - SliderInt: Current value ({}) is less than the new minimum value ({}). Setting current value to new minimum.", __FILE__, __LINE__, m_value, minimum);
		m_value = minimum;
	}
	else if (m_value > maximum)
	{
		EG_CORE_WARN("{}:{} - SliderInt: Current value ({}) is greater than the new maximum value ({}). Setting current value to new maximum.", __FILE__, __LINE__, m_value, maximum);
		m_value = maximum;
	}

	SliderChanged();
}
void SliderInt::SetValue(int value) noexcept
{
	if (m_value != value)
	{
		m_value = value;
		SliderChanged();

		SliderIntValueChangedEvent _e(m_value);
		m_OnValueChanged(this, _e);
	}
}
void SliderInt::SetTextInputHeight(float height) noexcept
{
	if (height <= 0.0f)
	{
		EG_CORE_ERROR("{}:{} - SliderInt: TextInput height must be greater than 0. IGNORING value ({}).", __FILE__, __LINE__, height);
		return;
	}
	m_valueTextInputOnRightHeight = height;
	SliderChanged();
}
void SliderInt::SetTextInputWidth(float width) noexcept
{
	if (width <= 0.0f)
	{
		EG_CORE_ERROR("{}:{} - SliderInt: TextInput width must be greater than 0. IGNORING value ({}).", __FILE__, __LINE__, width);
		return;
	}
	m_valueTextInputOnRightWidth = width;
	SliderChanged();
}
void SliderInt::SetTextInputHeightAndWidth(float height, float width) noexcept
{
	if (height <= 0.0f)
	{
		EG_CORE_ERROR("{}:{} - SliderInt: TextInput height must be greater than 0. IGNORING value ({}).", __FILE__, __LINE__, height);
	}
	else
	{
		m_valueTextInputOnRightHeight = height;
	}

	if (width <= 0.0f)
	{
		EG_CORE_ERROR("{}:{} - SliderInt: TextInput width must be greater than 0. IGNORING value ({}).", __FILE__, __LINE__, width);
	}
	else
	{
		m_valueTextInputOnRightWidth = width;
	}

	SliderChanged();
}
void SliderInt::SetLineBrushLeft(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Brush cannot be nullptr");
	EG_CORE_ASSERT(m_lineBrushLeft != nullptr, "No line brush left");

	m_lineBrushLeft = std::move(brush);
	m_lineBrushLeft->SetDrawRegion(m_allowedRegion);
}
void SliderInt::SetLineBrushRight(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Brush cannot be nullptr");
	EG_CORE_ASSERT(m_lineBrushRight != nullptr, "No line brush right");

	m_lineBrushRight = std::move(brush);
	m_lineBrushRight->SetDrawRegion(m_allowedRegion);
}
void SliderInt::SetCircleBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Brush cannot be nullptr");
	EG_CORE_ASSERT(m_circleBrush != nullptr, "No circle brush");

	m_circleBrush = std::move(brush);
	m_circleBrush->SetDrawRegion(m_allowedRegion);
}
void SliderInt::SetCircleBrushOuter(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Brush cannot be nullptr");
	EG_CORE_ASSERT(m_circleBrush2 != nullptr, "No circle brush 2");

	m_circleBrush2 = std::move(brush);
	m_circleBrush2->SetDrawRegion(m_allowedRegion);
}
void SliderInt::SetPopUpBackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Brush cannot be nullptr");
	EG_CORE_ASSERT(m_popUpBackgroundBrush != nullptr, "No pop up background brush");

	m_popUpBackgroundBrush = std::move(brush);
	m_popUpBackgroundBrush->SetDrawRegion(m_allowedRegion);
}
void SliderInt::SetPopUpBorderBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Brush cannot be nullptr");
	EG_CORE_ASSERT(m_popUpBorderBrush != nullptr, "No pop up border brush");

	m_popUpBorderBrush = std::move(brush);
	m_popUpBorderBrush->SetDrawRegion(m_allowedRegion);
}








void SliderInt::OnMouseMove(MouseMoveEvent& e)
{
	bool currentMouseIsOverCircle = CircleContainsPoint(e.GetX(), e.GetY());

	switch (m_mouseOverCircleState)
	{
	case MouseOverCircleState::DRAGGING:
	{
		if (currentMouseIsOverCircle)
		{
			if (!m_mouseIsOverCircle)
			{
				m_mouseIsOverCircle = true;
				m_OnMouseEnteredCircle(this, e);
			}
		}
		else
		{
			if (m_mouseIsOverCircle)
			{
				m_mouseIsOverCircle = false;
				m_OnMouseExitedCircle(this, e);
			}
		}

		if (e.GetX() > m_lineRightX)
		{
			m_circlePositionX = m_lineRightX;

			if (m_value != m_maxValue)
			{
				m_value = m_maxValue;
				UpdateValueTexts();

				SliderIntValueChangedEvent _e(m_value);
				m_OnValueChanged(this, _e);
			}
		}
		else if (e.GetX() < m_lineLeftX)
		{
			m_circlePositionX = m_lineLeftX;
			if (m_value != m_minValue)
			{
				m_value = m_minValue;
				UpdateValueTexts();

				SliderIntValueChangedEvent _e(m_value);
				m_OnValueChanged(this, _e);
			}
		}
		else
		{
			m_circlePositionX = e.GetX();
			int newValue = PixelToValue(m_circlePositionX);
			if (newValue != m_value)
			{
				m_value = newValue;
				UpdateValueTexts();

				SliderIntValueChangedEvent _e(m_value);
				m_OnValueChanged(this, _e);
			}
		}

		m_valueTextOnPopUp->AllowedRegion(GetPopUpRect());
		e.Handled(this);
		return;
	}
	case MouseOverCircleState::NOT_OVER:
	{
		if (currentMouseIsOverCircle)
		{
			m_mouseOverCircleState = MouseOverCircleState::OVER;
			m_OnMouseEnteredCircle(this, e);
			e.Handled(this);
			m_mouseIsOverCircle = currentMouseIsOverCircle;
			return;
		}

		break;
	}
	case MouseOverCircleState::OVER:
	{
		if (!currentMouseIsOverCircle)
		{
			m_mouseOverCircleState = MouseOverCircleState::NOT_OVER;
			m_OnMouseExitedCircle(this, e);
			break;
		}
		e.Handled(this);
		m_mouseIsOverCircle = currentMouseIsOverCircle;
		return;
	}
	}

	m_mouseIsOverCircle = currentMouseIsOverCircle;
	m_valueTextInputOnRight->OnMouseMove(e);
}
void SliderInt::MouseMoveHandledByPane(MouseMoveEvent& e)
{
	if (m_mouseOverCircleState == MouseOverCircleState::OVER)
	{
		m_mouseOverCircleState = MouseOverCircleState::NOT_OVER;
		m_mouseIsOverCircle = false;
		m_OnMouseExitedCircle(this, e);
	}
}
void SliderInt::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	if (m_mouseOverCircleState == MouseOverCircleState::OVER && e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		m_mouseOverCircleState = MouseOverCircleState::DRAGGING;
		m_OnBeginDragging(this, e);
		e.Handled(this);
		return;
	}

	m_valueTextInputOnRight->OnMouseButtonPressed(e);
}
void SliderInt::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	if (m_mouseOverCircleState == MouseOverCircleState::DRAGGING && e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		m_mouseOverCircleState = CircleContainsPoint(e.GetX(), e.GetY()) ? MouseOverCircleState::OVER : MouseOverCircleState::NOT_OVER;
		m_OnStoppedDragging(this, e);
		e.Handled(this);
		return;
	}

	m_valueTextInputOnRight->OnMouseButtonReleased(e);
}


bool SliderInt::CircleContainsPoint(float x, float y) const noexcept
{
	float delX = m_circlePositionX - x;
	float delY = m_lineY - y;
	float distance = std::sqrtf((delX * delX) + (delY * delY));
	return distance <= m_circleRadius;
}
}