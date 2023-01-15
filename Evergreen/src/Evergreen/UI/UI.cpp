#include "pch.h"
#include "UI.h"
#include "JSONLoading/ControlLoaders/TextLoader.h"
#include "JSONLoading/ControlLoaders/ButtonLoader.h"
#include "JSONLoading/ControlLoaders/RoundedButtonLoader.h"
#include "JSONLoading/ControlLoaders/ScrollableLayoutLoader.h"

#include <fstream>

#include "Brushes/SolidColorBrush.h"
#include "Brushes/GradientBrush.h"
#include "Brushes/RadialBrush.h"
#include "Brushes/BitmapBrush.h"

namespace Evergreen
{
UI::UI(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<Window> window) noexcept :
	m_deviceResources(deviceResources),
	m_window(window),
	m_rootLayout(nullptr),
	m_mouseHandlingControl(nullptr),
	m_mouseHandlingLayout(nullptr),
	m_keyboardHandlingControl(nullptr),
	m_keyboardHandlingLayout(nullptr)
{
	// Add built-in control loaders
	JSONLoaders::AddControlLoader("Text", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return TextLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("Button", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return ButtonLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("RoundedButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return RoundedButtonLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("ScrollableLayout", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return ScrollableLayoutLoader::Load(deviceResources, parentLayout, data, controlName); });

	// Add built-in style loaders
	JSONLoaders::AddStyleLoader("TextStyle", [](std::shared_ptr<DeviceResources> deviceResources, json& data, const std::string& styleName) -> std::unique_ptr<Style> { return std::move(TextStyleLoader::Load(deviceResources, data, styleName)); });

	LoadDefaultUI();
}

void UI::LoadDefaultUI() noexcept
{
	// TODO: Load a default UI that displays a "No defined layout"
	// Could make this quite elaborate, similar to how ImGui has the example of all controls
	// This could also contain links our website/documentation



	// TEST CODE
	m_rootLayout = std::make_unique<Layout>(
		m_deviceResources, 
		0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()),
		nullptr, 
		"root layout");
	
	Row* row0 = m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f });
	row0->BottomIsAdjustable(true);
	row0->MinHeight(110.0f);

	Row* row1 = m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f });
	row1->TopIsAdjustable(true);
	

	Column* column1 = m_rootLayout->AddColumn({ RowColumnType::FIXED, 100.0f });
	column1->RightIsAdjustable(true);
	
	Column* column2 = m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f });
	column2->LeftIsAdjustable(true);

	std::wstring textString = L"Some very long test text";
	std::unique_ptr<TextStyle> defaultStyle = std::make_unique<TextStyle>(m_deviceResources);
	std::unique_ptr<TextStyle> style = std::make_unique<TextStyle>(
		m_deviceResources,
		"Custom Style",
		Evergreen::FontFamily::Arial,
		22.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD
	);
	std::unique_ptr<SolidColorBrush> b1 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	Text* text = m_rootLayout->CreateControl<Text>(m_deviceResources, textString, std::move(b1), std::move(defaultStyle));

	RowColumnPosition position;
	position.Row = 1;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	std::wstring s = L"Custom Text";
	std::unique_ptr<SolidColorBrush> b2 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	Text* text2 = m_rootLayout->CreateControl<Text>(position, m_deviceResources, s, std::move(b2), std::move(style));



	// ================================================================================================
	RowColumnPosition sublayoutPosition;
	sublayoutPosition.Row = 0;
	sublayoutPosition.Column = 1;

	Layout* sublayout = m_rootLayout->AddSubLayout(sublayoutPosition);
	sublayout->AddRow({ RowColumnType::FIXED, 50.0f });
	Row* row = sublayout->AddRow({ RowColumnType::FIXED, 50.0f });
	row->BottomIsAdjustable(true);
	row->MinHeight(30.0f);
	row = sublayout->AddRow({ RowColumnType::STAR, 1.0f });
	row->TopIsAdjustable(true);
	row->MinHeight(100.0f);
	Column* column = sublayout->AddColumn({ RowColumnType::FIXED, 100.0f });
	column->RightIsAdjustable(true);
	column->MinWidth(75.0f);
	column = sublayout->AddColumn({ RowColumnType::STAR, 1.0f });
	column->LeftIsAdjustable(true);
	column->MinWidth(200.0f);






	RowColumnPosition buttonPosition;
	buttonPosition.Row = 0;
	buttonPosition.Column = 0;

	std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue));
	std::unique_ptr<SolidColorBrush> borderBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Red));

	Evergreen::Margin margin = { 10.0f, 10.0f, 10.0f, 10.0f };

	Button* button = sublayout->CreateControl<Button>(
		buttonPosition,
		m_deviceResources,
		std::move(backgroundBrush),
		std::move(borderBrush),
		3.0f,
		margin
	);

	Layout* buttonLayout = button->GetLayout();
	buttonLayout->AddRow({ RowColumnType::STAR, 1.0f });
	buttonLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	button->SetOnMouseEnteredButtonCallback(
		[](Control* control, Event& e) 
		{
			Button* b = static_cast<Button*>(control);
			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(b->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
			b->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button->SetOnMouseExitedButtonCallback(
		[](Control* control, Event& e)
		{
			Button* b = static_cast<Button*>(control);
			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(b->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue));
			b->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button->SetOnMouseLButtonDownCallback(
		[](Control* control, Event& e)
		{
			Button* b = static_cast<Button*>(control);
			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(b->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Purple));
			b->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button->SetOnMouseLButtonUpCallback(
		[](Control* control, Event& e)
		{
			Button* b = static_cast<Button*>(control);
			// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
			// OnMouseLeave event will fire and set the background color anyways)
			if (b->MouseIsOver())
			{
				std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(b->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
				b->BackgroundBrush(std::move(backgroundBrush));
			}

		}
	);







	RowColumnPosition buttonPosition2;
	buttonPosition2.Row = 1;
	buttonPosition2.Column = 0;

	std::unique_ptr<SolidColorBrush> backgroundBrush2 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Yellow));
	std::unique_ptr<SolidColorBrush> borderBrush2 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Red));

	Evergreen::Margin margin2 = { 10.0f, 10.0f, 10.0f, 10.0f };

	RoundedButton* button2 = sublayout->CreateControl<RoundedButton>(
		buttonPosition2,
		m_deviceResources,
		std::move(backgroundBrush2),
		std::move(borderBrush2),
		15.0f,
		15.0f,
		2.0f,
		margin2
	);

	Layout* buttonLayout2 = button2->GetLayout();
	buttonLayout2->AddRow({ RowColumnType::STAR, 1.0f });
	column = buttonLayout2->AddColumn({ RowColumnType::STAR, 1.0f });
	column->RightIsAdjustable(true);
	column->MinWidth(10.0f);
	column = buttonLayout2->AddColumn({ RowColumnType::STAR, 1.0f });
	column->LeftIsAdjustable(true);
	column->MinWidth(10.0f);

	Text* text3 = buttonLayout2->CreateControl<Text>(m_deviceResources);
	text3->SetText(L"nooooo...");
	text3->GetTextStyle()->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	text3->GetTextStyle()->TextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	
	button2->SetOnMouseEnteredButtonCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button2->SetOnMouseExitedButtonCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Yellow));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button2->SetOnMouseLButtonDownCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightBlue));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button2->SetOnMouseLButtonUpCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
			// OnMouseLeave event will fire and set the background color anyways)
			if (rb->MouseIsOver())
			{
				std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
				rb->BackgroundBrush(std::move(backgroundBrush));
			}

		}
	);
	button2->SetOnClickCallback(
		[](Control* b, Event& e)
		{
			static int iii = 0;

			RoundedButton* rb = static_cast<RoundedButton*>(b);

			Layout* layout = rb->GetLayout();

			Text* text = static_cast<Text*>(layout->GetControl(0));
			text->SetText(std::format(L"{}", iii));

			++iii;
		}
	);


	// --------------------------------------------------

	RowColumnPosition scrollableLayoutPosition;
	scrollableLayoutPosition.Row = 2;
	scrollableLayoutPosition.Column = 0;

	std::unique_ptr<SolidColorBrush> backgroundBrush3 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::LightSeaGreen));
	std::unique_ptr<SolidColorBrush> borderBrush3 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Red));

	Evergreen::Margin margin3 = { 5.0f, 5.0f, 5.0f, 5.0f };

	ScrollableLayout* scroll = sublayout->CreateControl<ScrollableLayout>(
		scrollableLayoutPosition,
		m_deviceResources,
		true, 
		true,
		std::move(backgroundBrush3), 
		std::move(borderBrush3), 
		2.0f, 
		margin3
	);
	scroll->VerticalScrollBarHiddenWhenNotOver(false);
	scroll->HorizontalScrollBarHiddenWhenNotOver(false);
	scroll->VerticalScrollBarCornerRadius(4.0f);
	scroll->HorizontalScrollBarCornerRadius(4.0f);

	scroll->AddColumn({ RowColumnType::FIXED, 50.0f });
	scroll->AddColumn({ RowColumnType::FIXED, 100.0f });


	std::unique_ptr<SolidColorBrush> scrollButtonBackgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Yellow));
	std::unique_ptr<SolidColorBrush> scrollButtonBorderBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Red));

	Evergreen::Margin scrollButtonMargin = { 2.0f, 2.0f, 2.0f, 2.0f };

	const int rows = 20;

	for (int iii = 0; iii < rows; ++iii)
		scroll->AddRow({ RowColumnType::FIXED, 20.0f });

	RoundedButton* scrollButton = scroll->CreateControl<RoundedButton>(
		m_deviceResources,
		std::move(scrollButtonBackgroundBrush),
		std::move(scrollButtonBorderBrush),
		0.0f,
		0.0f,
		1.0f,
		scrollButtonMargin
		);

	Layout* scrollButtonLayout = scrollButton->GetLayout();
	scrollButtonLayout->AddRow({ RowColumnType::STAR, 1.0f });
	scrollButtonLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	Text* scrollButtonText = scrollButtonLayout->CreateControl<Text>(m_deviceResources);
	scrollButtonText->SetText(L"nooooo123456789");
	scrollButtonText->GetTextStyle()->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	scrollButtonText->GetTextStyle()->TextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	scrollButton->SetOnMouseEnteredButtonCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	scrollButton->SetOnMouseExitedButtonCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Yellow));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	scrollButton->SetOnMouseLButtonDownCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightBlue));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	scrollButton->SetOnMouseLButtonUpCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
			// OnMouseLeave event will fire and set the background color anyways)
			if (rb->MouseIsOver())
			{
				std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
				rb->BackgroundBrush(std::move(backgroundBrush));
			}

		}
	);
	scrollButton->SetOnClickCallback(
		[](Control* b, Event& e)
		{
			static int iii = 0;

			RoundedButton* rb = static_cast<RoundedButton*>(b);

			Layout* layout = rb->GetLayout();

			Text* text = static_cast<Text*>(layout->GetControl(0));
			text->SetText(std::format(L"{}", iii));

			++iii;
		}
	);

	for (int iii = 1; iii < rows; ++iii)
	{
		RowColumnPosition pos;
		pos.Row = iii;
		pos.Column = 0;

		Text* t = scroll->CreateControl<Text>(pos, m_deviceResources);
		t->SetText(std::format(L"Yes {}", iii));
		t->GetTextStyle()->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		t->GetTextStyle()->TextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		pos.Column = 1;

		Text* t2 = scroll->CreateControl<Text>(pos, m_deviceResources);
		t2->SetText(std::format(L"No {}", iii));
		t2->GetTextStyle()->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		t2->GetTextStyle()->TextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}



	// Text Input -------------------------------------------------------------------------
	RowColumnPosition tiPosition;
	tiPosition.Row = 0;
	tiPosition.Column = 1;

	std::unique_ptr<SolidColorBrush> tiPlaceholderBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));
	std::unique_ptr<SolidColorBrush> tiInputBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	std::unique_ptr<TextStyle> tiPlaceholderStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"TextInput Placeholder Style",
		Evergreen::FontFamily::Arial,
		18.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD
	);
	std::unique_ptr<TextStyle> tiInputStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"TextInput Input Style",
		Evergreen::FontFamily::Arial,
		18.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD
	);

	std::unique_ptr<SolidColorBrush> tiBackgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
	std::unique_ptr<SolidColorBrush> tiBorderBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Purple));

	Evergreen::Margin tiMargin = { 5.0f, 5.0f, 15.0f, 5.0f };

	TextInput* ti = sublayout->CreateControl<TextInput>(
		tiPosition,
		m_deviceResources,
		L"placeholder text",
		std::move(tiPlaceholderBrush),
		std::move(tiPlaceholderStyle),
		std::move(tiInputBrush),
		std::move(tiInputStyle),
		std::move(tiBackgroundBrush),
		std::move(tiBorderBrush),
		2.0f,
		tiMargin
	);
}

void UI::LoadUI(const std::string& fileName) noexcept
{
	// Create a new root layout - this will destroy any layout that previously existed
	m_rootLayout = std::make_unique<Layout>(
		m_deviceResources, 
		0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()),
		nullptr,
		"Root Layout");

	if (!JSONLoaders::LoadUI(m_deviceResources, m_jsonRootDirectory, fileName, m_rootLayout.get()))
	{
		LoadErrorUI();
	}
}

void UI::LoadErrorUI() noexcept
{
	m_rootLayout = std::make_unique<Layout>(
		m_deviceResources, 
		0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()),
		nullptr, 
		"error UI root layout");
	m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f });
	m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f });
	
	std::unique_ptr<TextStyle> style = std::make_unique<TextStyle>(
		m_deviceResources,
		"Error Style",
		Evergreen::FontFamily::Arial,
		22.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_WHOLE_WORD
	);

	std::wstring text = L"Encountered error when parsing json file. See console output.";
	/*
	std::wstring message;

	for (const std::string& s : m_errorMessages)
		text = std::format(L"{}\n{}\n", text, std::wstring(s.begin(), s.end()));
	*/
	std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));
	
	m_rootLayout->CreateControl<Text>(m_deviceResources, text, std::move(brush), std::move(style));
}

void UI::Update() noexcept
{
	m_rootLayout->Update();
}

void UI::Render() const noexcept
{
	m_deviceResources->BeginDraw();

	m_rootLayout->Render();

	m_deviceResources->EndDraw();
}

void UI::OnChar(CharEvent& e) noexcept
{
	if (m_keyboardHandlingControl != nullptr)
	{
		m_keyboardHandlingControl->OnChar(e);
	}
	else if (m_keyboardHandlingLayout != nullptr)
	{
		m_keyboardHandlingLayout->OnChar(e);
	}

	if (!e.Handled())
		m_rootLayout->OnChar(e);

	m_keyboardHandlingControl = e.HandlingControl();
	m_keyboardHandlingLayout = e.HandlingLayout();
}
void UI::OnKeyPressed(KeyPressedEvent& e) noexcept
{

}
void UI::OnKeyReleased(KeyReleasedEvent& e) noexcept
{

}
void UI::OnWindowResize(WindowResizeEvent& e) noexcept
{
	m_rootLayout->Resize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
}
void UI::OnMouseMove(MouseMoveEvent& e) noexcept
{
	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseMove(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseMove(e);
	}

	if (!e.Handled())
		m_rootLayout->OnMouseMove(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}
void UI::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseButtonPressed(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseButtonPressed(e);
	}

	if (!e.Handled())
		m_rootLayout->OnMouseButtonPressed(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}
void UI::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseButtonReleased(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseButtonReleased(e);
	}

	if (!e.Handled())
		m_rootLayout->OnMouseButtonReleased(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}

void UI::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{
	m_rootLayout->OnMouseScrolledVertical(e);
}
void UI::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{
	m_rootLayout->OnMouseScrolledHorizontal(e);
}


}