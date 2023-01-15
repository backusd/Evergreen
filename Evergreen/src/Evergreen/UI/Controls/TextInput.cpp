#include "pch.h"
#include "TextInput.h"


namespace Evergreen
{
TextInput::TextInput(std::shared_ptr<DeviceResources> deviceResources,
						const D2D1_RECT_F& allowedRegion,
						const std::wstring& placeholderText,
						std::unique_ptr<ColorBrush> placeholderBrush,
						std::unique_ptr<TextStyle> placeholderStyle,
						std::unique_ptr<ColorBrush> inputTextBrush,
						std::unique_ptr<TextStyle> inputTextStyle,
						std::unique_ptr<ColorBrush> backgroundBrush,
						std::unique_ptr<ColorBrush> borderBrush,
						float borderWidth,
						const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, allowedRegion, margin),
	m_placeholderText(placeholderText),
	m_placeholderTextBrush(std::move(placeholderBrush)),
	m_placeholderTextStyle(std::move(placeholderStyle)),
	m_inputText(L""),
	m_inputTextBrush(std::move(inputTextBrush)),
	m_inputTextStyle(std::move(inputTextStyle)),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	m_backgroundRect({ 0.0f, 0.0f, 1000.0f, 1000.0f }), // dummy values that will be written over when allowed region is updated
	m_textInputControlIsSelected(false),
	m_nextCharIndex(0),
	m_mouseState(MouseOverState::NOT_OVER),
	m_drawVerticalBar(false)
{
	// Brushes
	if (m_placeholderTextBrush == nullptr)
		m_placeholderTextBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_inputTextBrush == nullptr)
		m_inputTextBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	m_verticalBarBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	// Text Styles
	if (m_placeholderTextStyle == nullptr)
	{
		m_placeholderTextStyle = std::make_unique<TextStyle>(
			m_deviceResources,
			"TextInput Placeholder Style",
			Evergreen::FontFamily::Calibri,
			12.0f,
			DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
			DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER, // Vertically align to the center of the layout
			DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
		);
	}

	if (m_inputTextStyle == nullptr)
	{
		m_inputTextStyle = std::make_unique<TextStyle>(
			m_deviceResources,
			"TextInput Input Style",
			Evergreen::FontFamily::Calibri,
			12.0f,
			DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
			DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER, // Vertically align to the center of the layout
			DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
			);
	}

	// Create the layout with a dummy size - will be updated when TextInputChanged() is called
	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		0.0f, 0.0f, 1000.0f, 1000.0f,
		nullptr, // Don't pass a brush to the layout - Button should draw the background, not the layout
		"TextInput Layout");

	// Add a single row and column to span the layout
	// NOTE: The user can optionally decide to add a second column by calling AddRightColumn() which will create
	//       a second column and a sublayout for Row:0 Column:1. They can then get that sublayout by calling GetRightColumnLayout()
	//       and can therefore put whatever they want in that layout
	m_layout->AddRow({ RowColumnType::STAR, 1.0f });
	m_layout->AddColumn({ RowColumnType::STAR, 1.0f });

	// Create the Text control that will be displayed in the TextInput layout
	m_text = m_layout->CreateControl<Text>(
		deviceResources,
		m_placeholderText,
		std::move(m_placeholderTextBrush->Duplicate()),
		std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(m_placeholderTextStyle->Duplicate().release())))
	);

	m_text->MarginLeft(4.0f);

	// Call TextInputChanged to update the layout
	TextInputChanged();

	// Set the initial text to be just the placeholder text
	SetTextToPlaceholder();
}

void TextInput::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	// Draw the background
	m_deviceResources->D2DDeviceContext()->FillRectangle(m_backgroundRect, m_backgroundBrush->Get());

	// Have the layout draw the background and text
	m_layout->Render();

	// Draw vertical bar
	if (m_drawVerticalBar)
	{
		m_deviceResources->D2DDeviceContext()->DrawLine(
			D2D1::Point2F(m_verticalBarX, m_verticalBarTop),
			D2D1::Point2F(m_verticalBarX, m_verticalBarBottom),
			m_verticalBarBrush->Get()
		);
	}

	// Draw the border last so it appears on top
	if (m_borderWidth > 0.0f)
		m_deviceResources->D2DDeviceContext()->DrawRectangle(m_backgroundRect, m_borderBrush->Get(), m_borderWidth);
}

void TextInput::TextInputChanged() noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_text != nullptr, "No Text control");

	// Update the background rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	m_layout->Resize(m_backgroundRect);

	// Update the location of the vertical bar
	UpdateVerticalBar();
}

void TextInput::UpdateVerticalBar() noexcept
{
	EG_CORE_ASSERT(m_text != nullptr, "No Text control");

	m_verticalBarTop = m_text->Top();
	m_verticalBarBottom = m_text->Bottom();

	if (m_nextCharIndex == m_text->Size())
		m_verticalBarX = m_text->Right();
	else
		m_verticalBarX = m_text->RightSideOfCharacterAtIndex(m_nextCharIndex) + 1.0f;
}

void TextInput::OnMarginChanged()
{
	TextInputChanged();
}
void TextInput::OnAllowedRegionChanged()
{
	TextInputChanged();
}

void TextInput::SetTextToPlaceholder() noexcept
{
	EG_CORE_ASSERT(m_text != nullptr, "Text control should not be nullptr");
	EG_CORE_ASSERT(m_placeholderTextBrush != nullptr, "Placeholder brush should not be nullptr");
	EG_CORE_ASSERT(m_placeholderTextStyle != nullptr, "Placeholder text style should not be nullptr");

	m_text->SetText(m_placeholderText);
	m_text->SetColorBrush(std::move(m_placeholderTextBrush->Duplicate()));
	m_text->SetTextStyle(std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(m_placeholderTextStyle->Duplicate().release()))));

	m_drawVerticalBar = false;
}
void TextInput::SetTextToInput() noexcept
{
	EG_CORE_ASSERT(m_text != nullptr, "Text control should not be nullptr");
	EG_CORE_ASSERT(m_inputTextBrush != nullptr, "Input brush should not be nullptr");
	EG_CORE_ASSERT(m_inputTextStyle != nullptr, "Input text style should not be nullptr");

	m_text->SetText(m_inputText);
	m_text->SetColorBrush(std::move(m_inputTextBrush->Duplicate()));
	m_text->SetTextStyle(std::move(std::unique_ptr<TextStyle>(static_cast<TextStyle*>(m_inputTextStyle->Duplicate().release()))));

	m_drawVerticalBar = true;
	UpdateVerticalBar();
}


void TextInput::OnChar(CharEvent& e) noexcept
{
	// Only edit the text if this control has been clicked into
	if (m_textInputControlIsSelected)
	{
		const char key = e.GetKeyCode();
		// First, handle the backspace character
		if (key == '\b')
		{
			if (m_nextCharIndex > 0)
			{
				m_inputText.erase(--m_nextCharIndex, 1);
				m_text->RemoveChar(m_nextCharIndex);
				UpdateVerticalBar();
			}
			return;
		}

		// Update the input text string that is held locally (we need this to be able to swap back and
		// forth between the input text and placeholder text)
		if (m_nextCharIndex == m_inputText.size())
			m_inputText.push_back(key);
		else
			m_inputText.insert(m_nextCharIndex, 1, key);

		// Next, update Text control (Faster than calling SetText)
		m_text->AddChar(key, m_nextCharIndex);
		UpdateVerticalBar();

		++m_nextCharIndex;
	}
}
void TextInput::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	// Not currently handling key pressed events...
}
void TextInput::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	// Only edit the text if this control has been clicked into
	if (m_textInputControlIsSelected)
	{
		switch (e.GetKeyCode())
		{
		case KEY_CODE::EG_LEFT_ARROW: break;
		case KEY_CODE::EG_RIGHT_ARROW: break;
		}
	}
}
void TextInput::OnMouseMove(MouseMoveEvent& e) noexcept
{
	// First pass to layout, if the layout does not handle it, then the control can handle it
	m_layout->OnMouseMove(e);
	if (e.Handled())
		return;

	bool mouseIsOver = ContainsPoint(e.GetX(), e.GetY());

	switch (m_mouseState)
	{
	case MouseOverState::NOT_OVER:
		if (mouseIsOver)
		{
			m_mouseState = MouseOverState::OVER;
			e.Handled(this);
		}

		break;
	case MouseOverState::OVER:
		if (!mouseIsOver)
		{
			m_mouseState = MouseOverState::NOT_OVER;
			return;
		}

		e.Handled(this);
		break;
	case MouseOverState::OVER_AND_LBUTTON_DOWN:
		if (!mouseIsOver)
		{
			m_mouseState = MouseOverState::NOT_OVER_AND_LBUTTON_DOWN;
		}

		e.Handled(this);
		break;
	case MouseOverState::NOT_OVER_AND_LBUTTON_DOWN:
		if (mouseIsOver)
		{
			m_mouseState = MouseOverState::OVER_AND_LBUTTON_DOWN;
		}
		e.Handled(this);
		break;
	}
}
void TextInput::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{

}
void TextInput::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{

}
void TextInput::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	// First, handle the scenario when a user clicks out of the control
	if (m_textInputControlIsSelected && m_mouseState == MouseOverState::NOT_OVER)
	{
		// Don't care about what button was pressed - We will just say that all buttons are allowed to click out of a TextInput
		m_textInputControlIsSelected = false;

		// if the input text size is 0, reload the placeholder text
		if (m_inputText.size() == 0)
			SetTextToPlaceholder();

		return;
	}

	// Next pass to layout, if the layout does not handle it, then the control can handle it
	m_layout->OnMouseButtonPressed(e);
	if (e.Handled())
		return;

	// Only process the event if the mouse is over the control and if its the LButton
	if (m_mouseState == MouseOverState::OVER)
	{
		// As long as the mouse is still over the control, we want to consider the event to be handled,
		// even if we don't actually do anything
		e.Handled(this);

		if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
		{
			m_mouseState = MouseOverState::OVER_AND_LBUTTON_DOWN;			
		}
	}
}
void TextInput::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	// Before passing to layout, first handle this event if in the state OVER_AND_LBUTTON_DOWN

	if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		if (m_mouseState == MouseOverState::OVER_AND_LBUTTON_DOWN)
		{
			m_mouseState = MouseOverState::OVER;
			if (!m_textInputControlIsSelected)
			{
				m_textInputControlIsSelected = true;

				if (m_inputText.size() == 0)
					SetTextToInput();
			}
			
			e.Handled(this);
			return;
		}
		else if (m_mouseState == MouseOverState::NOT_OVER_AND_LBUTTON_DOWN)
		{
			m_mouseState = MouseOverState::NOT_OVER;
			return;
		}
	}
}
void TextInput::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{

}

bool TextInput::ContainsPoint(float x, float y) const noexcept
{
	return m_layout->ContainsPoint(x, y);
}



}