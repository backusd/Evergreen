#include "pch.h"
#include "TextInput.h"


namespace Evergreen
{
const float TextInput::m_originalMarginLeft = 4.0f;

TextInput::TextInput(std::shared_ptr<DeviceResources> deviceResources,
					UI* ui,
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
	Control(deviceResources, ui, allowedRegion, margin),
	m_rightSublayout(nullptr),
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
	m_drawVerticalBar(false),
	m_marginLeft(m_originalMarginLeft),
	m_verticalBarWidth(2.0f)
{
	// Brushes
	if (m_placeholderTextBrush == nullptr)
		m_placeholderTextBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_inputTextBrush == nullptr)
		m_inputTextBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

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
	else
	{
		// We must enforce the text style to be no wrap
		m_placeholderTextStyle->WordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP);
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
	else
	{
		// We must enforce the text style to be no wrap
		m_inputTextStyle->WordWrapping(DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP);
	}

	// Create the layout with a dummy size - will be updated when TextInputChanged() is called
	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		ui,
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

	m_text->MarginLeft(m_marginLeft); 
	m_text->MarginRight(4.0f);

	// Call TextInputChanged to update the layout
	TextInputChanged();

	// Set the initial text to be just the placeholder text
	SetTextToPlaceholder();
}

void TextInput::Render() const
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
			m_verticalBarBrush->Get(),
			m_verticalBarWidth
		);
	}

	// Draw the border last so it appears on top
	if (m_borderWidth > 0.0f)
		m_deviceResources->D2DDeviceContext()->DrawRectangle(m_backgroundRect, m_borderBrush->Get(), m_borderWidth);
}

Layout* TextInput::AddRightColumnLayout(RowColumnDefinition rightColumnDefinition)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	if (m_rightSublayout == nullptr)
	{
		m_layout->AddColumn(rightColumnDefinition);

		RowColumnPosition pos;
		pos.Row = 0;
		pos.Column = 1;
		pos.RowSpan = 1;
		pos.ColumnSpan = 1;

		m_rightSublayout = m_layout->AddSubLayout(pos);

		// Need to adjust the right side of the text region so it doesn't bleed into the right column
		m_textRegionRect.right = m_layout->Columns()[0].Right();
	}
	else
	{
		EG_CORE_WARN("{}:{} - AddRightColumnLayout(): A right column layout has already been added to this TextInput", __FILE__, __LINE__);
	}
	
	return m_rightSublayout;
}
Layout* TextInput::GetRightColumnLayout() const noexcept
{
	EG_CORE_ASSERT(m_rightSublayout != nullptr, "Right column layout has not been added");
	return m_rightSublayout;
}

void TextInput::TextInputChanged() noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// Update the background rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	m_layout->Resize(m_backgroundRect);

	// The text region is always row 0/column 0 of the TextInput control (and we only allow there to be a single row. If content,
	// is added to the TextInput, it will be added as a sublayout at row 0 column 1)
	m_textRegionRect.left = m_backgroundRect.left;
	m_textRegionRect.top = m_backgroundRect.top;
	m_textRegionRect.bottom = m_backgroundRect.bottom;
	m_textRegionRect.right = m_layout->Columns()[0].Right();

	// Update the location of the vertical bar
	UpdateVerticalBar();
}

void TextInput::UpdateVerticalBar() noexcept
{
	EG_CORE_ASSERT(m_text != nullptr, "No Text control");

	m_verticalBarTop = m_text->Top();
	m_verticalBarBottom = m_text->Bottom();

	if (m_nextCharIndex == m_text->Size())
		m_verticalBarX = m_text->Right() + 1.0f;
	else if (m_nextCharIndex == 0)
		m_verticalBarX = m_text->Left();
	else
		m_verticalBarX = m_text->RightSideOfCharacterAtIndex(m_nextCharIndex - 1);
}

void TextInput::OnMarginChanged()
{
	TextInputChanged();
}
void TextInput::OnAllowedRegionChanged()
{
	EG_CORE_ASSERT(m_text != nullptr, "No Text");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");
	EG_CORE_ASSERT(m_placeholderTextBrush != nullptr, "No placeholder text brush");
	EG_CORE_ASSERT(m_inputTextBrush != nullptr, "No input text brush");
	EG_CORE_ASSERT(m_verticalBarBrush != nullptr, "No vertical bar brush");

	TextInputChanged();

	// Reset the text margin and vertical bar
	m_marginLeft = m_originalMarginLeft;
	m_text->MarginLeft(m_marginLeft);

	// I suppose it is probably possible that the size of the input string could change
	// as the allowed region changes. So to be safe, just set the nextCharIndex to the end
	// of the current string
	m_nextCharIndex = static_cast<unsigned int>(m_inputText.size());

	// Update the location of the vertical bar
	UpdateVerticalBar();

	// Update brush regions
	m_backgroundBrush->SetDrawRegion(m_allowedRegion);
	m_borderBrush->SetDrawRegion(m_allowedRegion);
	m_placeholderTextBrush->SetDrawRegion(m_allowedRegion);
	m_inputTextBrush->SetDrawRegion(m_allowedRegion);
	m_verticalBarBrush->SetDrawRegion(m_allowedRegion);
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

	UpdateVerticalBar();
}

void TextInput::OnChar(CharEvent& e)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_text != nullptr, "No Text");

	// Only edit the text if this control has been clicked into
	if (m_textInputControlIsSelected)
	{
		const char key = e.GetKeyCode();

		// First, handle if the user presses ENTER
		if (key == '\r' || key == '\n')
		{
			m_OnEnterKey(this, e);
			return;
		}

		// Second, handle the backspace character
		if (key == '\b')
		{
			if (m_nextCharIndex > 0)
			{
				m_inputText.erase(--m_nextCharIndex, 1);
				m_text->RemoveChar(m_nextCharIndex);

				// If we have updated the left margin, we may need to update it again to shift the text back towards the right
				if (m_marginLeft != m_originalMarginLeft)
				{
					// If the text takes up less than half of the available space, adjust the margin
					const float halfTextRegionWidth = ((m_textRegionRect.right - m_textRegionRect.left) / 2.0f);
					const float middleOfTextRegion = m_textRegionRect.left + halfTextRegionWidth;
					if (m_text->Right() < middleOfTextRegion)
					{
						m_marginLeft = std::min(m_originalMarginLeft, m_marginLeft + (halfTextRegionWidth / 2.0f)); // Add back one quarter of the available space
						m_text->MarginLeft(m_marginLeft);
					}
				}			

				// Update the vertical bar location
				UpdateVerticalBar();

				// If the vertical bar is left of the text region, adjust the margin
				if (m_verticalBarX < m_textRegionRect.left)
				{
					const float quarterTextRegionWidth = ((m_textRegionRect.right - m_textRegionRect.left) / 4.0f);
					m_marginLeft = std::min(m_originalMarginLeft, m_marginLeft + quarterTextRegionWidth); // Add back one quarter of the available space
					m_text->MarginLeft(m_marginLeft);
					UpdateVerticalBar();
				}
			}
		}
		else
		{
			// Update the input text string that is held locally (we need this to be able to swap back and
			// forth between the input text and placeholder text)
			if (m_nextCharIndex == m_inputText.size())
				m_inputText.push_back(key);
			else
				m_inputText.insert(m_nextCharIndex, 1, key);

			// Next, update Text control (Faster than calling SetText)
			m_text->AddChar(key, m_nextCharIndex);
			++m_nextCharIndex;

			UpdateVerticalBar();

			// If the text has surpassed the right side of the input control, move the left margin to keep the right side
			// of the text aligned with the right side of the control area
			if (m_verticalBarX > m_textRegionRect.right)
			{
				m_marginLeft = -1.0f * (m_text->Width() - (m_textRegionRect.right - m_textRegionRect.left - 4.0f));
				m_text->MarginLeft(m_marginLeft);
				UpdateVerticalBar();
			}
		}

		m_OnInputTextChanged(this, e);
	}
	else
	{
		m_layout->OnChar(e);
	}
}
void TextInput::OnKeyPressed(KeyPressedEvent& e)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_text != nullptr, "No Text");

	// Going to process the arrow keys here because when an arrow is held down, only the OnKeyPressed event is
	// repeated. OnKeyReleased only occurs once when the key is actually released

	// Only edit the text if this control has been clicked into
	if (m_textInputControlIsSelected)
	{
		switch (e.GetKeyCode())
		{
		case KEY_CODE::EG_LEFT_ARROW:
		{
			if (m_nextCharIndex > 0)
			{
				--m_nextCharIndex;
				UpdateVerticalBar();

				// If the vertical bar is left of the text region, adjust the margin
				if (m_verticalBarX < m_textRegionRect.left)
				{
					const float quarterTextRegionWidth = ((m_textRegionRect.right - m_textRegionRect.left) / 4.0f);
					m_marginLeft = std::min(m_originalMarginLeft, m_marginLeft + quarterTextRegionWidth); // Add back one quarter of the available space
					m_text->MarginLeft(m_marginLeft);
					UpdateVerticalBar();
				}
			}
			break;
		}
		case KEY_CODE::EG_RIGHT_ARROW:
		{
			if (m_nextCharIndex < m_text->Size())
			{
				++m_nextCharIndex;
				UpdateVerticalBar();

				// If the vertical bar is right of the text region, adjust the margin
				if (m_verticalBarX > m_textRegionRect.right)
				{
					const float quarterTextRegionWidth = ((m_textRegionRect.right - m_textRegionRect.left) / 4.0f);
					m_marginLeft = std::max(m_marginLeft - quarterTextRegionWidth, -1.0f * (m_text->Width() - (m_textRegionRect.right - m_textRegionRect.left - 4.0f))); // Add back one quarter of the available space
					m_text->MarginLeft(m_marginLeft);
					UpdateVerticalBar();
				}
			}
			break;
		}
		}
	}
	else
	{
		m_layout->OnKeyPressed(e);
	}
}
void TextInput::OnKeyReleased(KeyReleasedEvent& e)
{

}
void TextInput::OnMouseMove(MouseMoveEvent& e)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	bool mouseIsOver = ContainsPoint(e.GetX(), e.GetY());

	// We are going to just trigger the OnMouseMove callback as long as the mouse is moving over the
	// control, even though the layout may decide to handle the event
	if (mouseIsOver)
		m_OnMouseMoved(this, e);

	// First pass to layout, if the layout does not handle it, then the control can handle it
	m_layout->OnMouseMove(e);
	if (e.Handled())
		return;

	switch (m_mouseState)
	{
	case MouseOverState::NOT_OVER:
		if (mouseIsOver)
		{
			m_mouseState = MouseOverState::OVER;
			e.Handled(this);
			Window::SetCursor(Cursor::I_BEAM);
			m_OnMouseEntered(this, e);
		}
		else if (m_textInputControlIsSelected)
		{
			// The TextInput is selected so just continue to capture the mouse
			e.Handled(this);
		}

		break;
	case MouseOverState::OVER:
		if (!mouseIsOver)
		{
			m_mouseState = MouseOverState::NOT_OVER;
			Window::SetCursor(Cursor::ARROW);
			m_OnMouseExited(this, e);
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
void TextInput::OnMouseScrolledVertical(MouseScrolledEvent& e)
{

}
void TextInput::OnMouseScrolledHorizontal(MouseScrolledEvent& e)
{

}
void TextInput::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// First, handle the scenario when a user clicks out of the control
	if (m_textInputControlIsSelected && m_mouseState == MouseOverState::NOT_OVER)
	{
		// Don't care about what button was pressed - We will just say that all buttons are allowed to click out of a TextInput
		m_textInputControlIsSelected = false;

		// Hide the vertical bar
		m_drawVerticalBar = false;

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
			m_OnMouseLButtonDown(this, e);
		}
	}
}
void TextInput::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_text != nullptr, "No Text");

	// Before passing to layout, first handle this event if in the state OVER_AND_LBUTTON_DOWN

	if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		if (m_mouseState == MouseOverState::OVER_AND_LBUTTON_DOWN)
		{
			m_mouseState = MouseOverState::OVER;
			if (!m_textInputControlIsSelected)
			{
				m_textInputControlIsSelected = true;

				// Show the vertical bar
				m_drawVerticalBar = true;

				// Only need to call SetTextToInput if we are switching back from placeholder which is only true if there is no input text
				if (m_inputText.size() == 0)
					SetTextToInput();
			}

			// Update the location of the vertical bar based on where the user clicked
			float x = e.GetX();
			if (m_text->Right() < x) // If the user clicks right all the text, just set m_nextCharIndex to the end
				m_nextCharIndex = m_text->Size();
			else
			{
				for (unsigned int iii = 0; iii < m_text->Size(); ++iii)
				{
					if (m_text->RightSideOfCharacterAtIndex(iii) - 2.0f > x)
					{
						m_nextCharIndex = iii;
						break;
					}
				}
			}

			UpdateVerticalBar();
			
			e.Handled(this);
			m_OnMouseLButtonUp(this, e);
			m_OnClick(this, e);
			return;
		}
		else if (m_mouseState == MouseOverState::NOT_OVER_AND_LBUTTON_DOWN)
		{
			m_mouseState = MouseOverState::NOT_OVER;
			m_OnMouseLButtonUp(this, e);
			m_OnClick(this, e);
			return;
		}
	}

	// Finally, pass to layout
	m_layout->OnMouseButtonReleased(e);
}
void TextInput::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e)
{

}

bool TextInput::ContainsPoint(float x, float y) const noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	return m_layout->ContainsPoint(x, y);
}

void TextInput::SetPlaceholderText(const std::wstring& placeholderText) noexcept
{
	m_placeholderText = placeholderText;

	// If there is no input text and the control is not selected, display the placeholder text
	if (m_inputText.size() == 0 && !m_textInputControlIsSelected)
	{
		SetTextToPlaceholder();
	}
}
void TextInput::SetPlaceholderTextStyle(std::unique_ptr<TextStyle> style) noexcept
{
	EG_CORE_ASSERT(style != nullptr, "Cannot set placeholder style to nullptr");

	m_placeholderTextStyle = std::move(style);

	// If there is no input text and the control is not selected, display the placeholder text
	if (m_inputText.size() == 0 && !m_textInputControlIsSelected)
	{
		SetTextToPlaceholder();
	}
}
void TextInput::SetPlaceholderTextBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Cannot set placeholder brush to nullptr");

	m_placeholderTextBrush = std::move(brush);

	// If there is no input text and the control is not selected, display the placeholder text
	if (m_inputText.size() == 0 && !m_textInputControlIsSelected)
	{
		SetTextToPlaceholder();
	}
}
void TextInput::SetInputText(const std::wstring& inputText) noexcept
{
	m_inputText = inputText;

	if (inputText.size() > 0)
	{		
		m_nextCharIndex = static_cast<unsigned int>(m_inputText.size());
		SetTextToInput();
	}
	else if (!m_textInputControlIsSelected)
	{
		SetTextToPlaceholder();
	}
}
void TextInput::SetInputTextStyle(std::unique_ptr<TextStyle> style) noexcept
{
	EG_CORE_ASSERT(style != nullptr, "Cannot set input style to nullptr");

	m_inputTextStyle = std::move(style);

	if (m_inputText.size() > 0 || m_textInputControlIsSelected)
	{
		SetTextToInput();
	}
}
void TextInput::SetInputTextBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Cannot set input brush to nullptr");

	m_inputTextBrush = std::move(brush);

	if (m_inputText.size() > 0 || m_textInputControlIsSelected)
	{
		SetTextToInput();
	}
}
void TextInput::SetBackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Cannot set background brush to nullptr");

	m_backgroundBrush = std::move(brush);	
	m_backgroundBrush->SetDrawRegion(m_allowedRegion);
}
void TextInput::SetBorderBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Cannot set border brush to nullptr");

	m_borderBrush = std::move(brush);
	m_borderBrush->SetDrawRegion(m_allowedRegion);
}
void TextInput::SetBorderWidth(float width) noexcept
{
	m_borderWidth = width;
}
void TextInput::SetVerticalBarBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "Cannot set vertical bar brush to nullptr");

	m_verticalBarBrush = std::move(brush);
	m_verticalBarBrush->SetDrawRegion(m_allowedRegion);
}
void TextInput::SetVerticalBarWidth(float width) noexcept
{
	m_verticalBarWidth = width;
}

Control* TextInput::GetControlByName(const std::string& name) noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	if (m_name.compare(name) == 0)
		return this;

	return m_layout->GetControlByName(name);
}
Control* TextInput::GetControlByID(unsigned int id) noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	if (m_id == id)
		return this;

	return m_layout->GetControlByID(id);
}

}