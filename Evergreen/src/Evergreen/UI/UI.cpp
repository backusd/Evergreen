#include "pch.h"
#include "UI.h"
#include "JSONLoading/ControlLoaders/TextLoader.h"
#include "JSONLoading/ControlLoaders/ButtonLoader.h"
#include "JSONLoading/ControlLoaders/RoundedButtonLoader.h"
#include "JSONLoading/ControlLoaders/ScrollableLayoutLoader.h"
#include "JSONLoading/ControlLoaders/TextInputLoader.h"

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
	JSONLoaders::AddControlLoader("TextInput", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return TextInputLoader::Load(deviceResources, parentLayout, data, controlName); });

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
		this,
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

	button->SetOnMouseEnteredCallback(
		[](Control* control, Event& e) 
		{
			Button* b = static_cast<Button*>(control);
			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(b->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
			b->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button->SetOnMouseExitedCallback(
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
	
	button2->SetOnMouseEnteredCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	button2->SetOnMouseExitedCallback(
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

	scrollButton->SetOnMouseEnteredCallback(
		[](Control* b, Event& e)
		{
			RoundedButton* rb = static_cast<RoundedButton*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(rb->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
			rb->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	scrollButton->SetOnMouseExitedCallback(
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
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
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
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
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
	ti->Name("TextInputControl");

	Layout* tiRightLayout = ti->AddRightColumnLayout({ RowColumnType::FIXED, 50.0f });
	tiRightLayout->AddRow({ RowColumnType::STAR, 1.0f });
	tiRightLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	std::unique_ptr<SolidColorBrush> tiRightButtonBackgroundBrush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	std::unique_ptr<SolidColorBrush> tiRightButtonBorderBrush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Pink));
	Evergreen::Margin tiRightButtonMargin = { 4.0f, 4.0f, 4.0f, 4.0f };

	Button* tiRightButton = tiRightLayout->CreateControl<Button>(
		m_deviceResources,
		std::move(tiRightButtonBackgroundBrush),
		std::move(tiRightButtonBorderBrush),
		0.0f,
		tiRightButtonMargin
		);

	Layout* tiRightButtonLayout = tiRightButton->GetLayout();
	tiRightButtonLayout->AddRow({ RowColumnType::STAR, 1.0f });
	tiRightButtonLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	Text* tiRightButtonText = tiRightButtonLayout->CreateControl<Text>(m_deviceResources);

	std::wstring tiRightButtonTextString = L"";
	tiRightButtonTextString.push_back(static_cast<wchar_t>(std::stoi(L"0xE721", nullptr, 16)));

	tiRightButtonText->SetText(tiRightButtonTextString);
	tiRightButtonText->GetTextStyle()->ParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	tiRightButtonText->GetTextStyle()->TextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	tiRightButtonText->GetTextStyle()->FontFamily(FontFamily::Segoe_MDL2_Assets);
	tiRightButtonText->GetTextStyle()->FontSize(22.0f);

	tiRightButton->SetOnMouseEnteredCallback(
		[](Control* b, Event& e)
		{
			Button* button = static_cast<Button*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
			button->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	tiRightButton->SetOnMouseExitedCallback(
		[](Control* b, Event& e)
		{
			Button* button = static_cast<Button*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
			button->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	tiRightButton->SetOnMouseLButtonDownCallback(
		[](Control* b, Event& e)
		{
			Button* button = static_cast<Button*>(b);

			std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightBlue));
			button->BackgroundBrush(std::move(backgroundBrush));
		}
	);
	tiRightButton->SetOnMouseLButtonUpCallback(
		[](Control* b, Event& e)
		{
			Button* button = static_cast<Button*>(b);

			// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
			// OnMouseLeave event will fire and set the background color anyways)
			if (button->MouseIsOver())
			{
				std::unique_ptr<SolidColorBrush> backgroundBrush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::AliceBlue));
				button->BackgroundBrush(std::move(backgroundBrush));
			}

		}
	);
	tiRightButton->SetOnClickCallback(
		[this](Control* b, Event& e)
		{
			Control* _textInput = this->GetControlByName("TextInputControl");
			TextInput* textInput = static_cast<TextInput*>(_textInput);

			static int iii = 0;
			textInput->SetInputText(std::format(L"{}", ++iii));
			
			textInput->SetPlaceholderText(L"baby cakes");
			textInput->ActivateForTextInput();

			std::unique_ptr<SolidColorBrush> inputBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Purple));
			textInput->SetInputTextBrush(std::move(inputBrush));

			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));

			std::unique_ptr<SolidColorBrush> borderBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkGreen));
			textInput->SetBorderBrush(std::move(borderBrush));

			textInput->SetBorderWidth(6.0f);
		}
	);
	// ---
	ti->SetOnMouseEnteredCallback(
		[](Control* c, Event& e)
		{
			TextInput* textInput = static_cast<TextInput*>(c);
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);

	ti->SetOnMouseExitedCallback(
		[](Control* c, Event& e)
		{
			TextInput* textInput = static_cast<TextInput*>(c);
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);

	ti->SetOnMouseLButtonDownCallback(
		[](Control* c, Event& e)
		{
			TextInput* textInput = static_cast<TextInput*>(c);
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Coral));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);

	ti->SetOnMouseLButtonUpCallback(
		[](Control* c, Event& e)
		{
			TextInput* textInput = static_cast<TextInput*>(c);
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);

	ti->SetOnEnterKeyCallback(
		[](Control* c, Event& e)
		{
			TextInput* textInput = static_cast<TextInput*>(c);
			//EG_CORE_TRACE("{}", "TextInput: ENTER");
		}
	);

	ti->SetOnInputTextChangedCallback(
		[](Control* c, Event& e)
		{
			TextInput* textInput = static_cast<TextInput*>(c);
			//std::string s(textInput->GetInputText().begin(), textInput->GetInputText().end());
			//EG_CORE_TRACE("{}", s);
		}
	);


	// ========================================================================================
	// Pane
	//

	std::unique_ptr<Pane> pane = std::make_unique<Pane>(
		m_deviceResources,
		this,
		150.0f, 150.0f, 500.0f, 700.0f,
		true, true, 
		nullptr, nullptr, 1.0f,
		true, nullptr
		);

	pane->AddRow({ RowColumnType::STAR, 1.0f });
	pane->AddColumn({ RowColumnType::STAR, 1.0f });
	pane->SetCornerRadius(8.0f);

	Layout* titleLayout = pane->GetTitleBarLayout();
	titleLayout->AddRow({ RowColumnType::STAR, 1.0f });
	titleLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	pane->ClearTitleBarLayoutAndAddTitle("Some title bitch");

	m_panes.push_back(std::move(pane));

	pane = std::make_unique<Pane>(
		m_deviceResources,
		this,
		50.0f, 50.0f, 100.0f, 100.0f,
		true, true,
		nullptr, nullptr, 1.0f,
		true, nullptr
		);

	pane->AddRow({ RowColumnType::STAR, 1.0f });
	pane->AddColumn({ RowColumnType::STAR, 1.0f });

	titleLayout = pane->GetTitleBarLayout();
	titleLayout->AddRow({ RowColumnType::STAR, 1.0f });
	titleLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	pane->ClearTitleBarLayoutAndAddTitle("Second Pane");

	m_panes.push_back(std::move(pane));
}

void UI::LoadUI(const std::string& fileName) noexcept
{
	// Create a new root layout - this will destroy any layout that previously existed
	m_rootLayout = std::make_unique<Layout>(
		m_deviceResources, 
		this,
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
		this,
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

	// Iterate of the panes in reverse order so that we render the ones on top last
	for (auto iter = m_panes.rbegin(); iter != m_panes.rend(); ++iter) 
	{
		iter->get()->Render();
	}

	m_deviceResources->EndDraw();
}

void UI::RemovePane(Pane* pane) noexcept
{
	// Get the iterator to the input Pane parameter
	auto p = std::find_if(m_panes.begin(), m_panes.end(),
		[&pane](const std::unique_ptr<Pane>& item) -> bool
		{
			return pane == item.get();
		}
	);

	if (p != m_panes.end()) {
		// It is possible (if not likely) that the mouse handling control is within the pane
		// So this needs to be cleared out before we can remove the pane, otherwise, the mouse handling
		// control pointer will point to a control that does not exist any more
		m_mouseHandlingControl = nullptr;
		m_mouseHandlingLayout = nullptr;
		m_keyboardHandlingControl = nullptr;
		m_keyboardHandlingLayout = nullptr;

		m_panes.erase(p);
	}
}
void UI::BringPaneToForeground(Pane* pane) noexcept
{
	// Get the iterator to the input Pane parameter
	auto p = std::find_if(m_panes.begin(), m_panes.end(),
		[&pane](const std::unique_ptr<Pane>& item) -> bool
		{
			return pane == item.get();
		}
	);

	// Put the pane at the front of the m_panes vector
	if (p != m_panes.end()) {
		std::rotate(m_panes.begin(), p, p + 1);
	}
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
	
	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnChar(e);
	}

	if (!e.Handled())
		m_rootLayout->OnChar(e);

	m_keyboardHandlingControl = e.HandlingControl();
	m_keyboardHandlingLayout = e.HandlingLayout();
}
void UI::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	if (m_keyboardHandlingControl != nullptr)
	{
		m_keyboardHandlingControl->OnKeyPressed(e);
	}
	else if (m_keyboardHandlingLayout != nullptr)
	{
		m_keyboardHandlingLayout->OnKeyPressed(e);
	}

	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnKeyPressed(e);
	}

	if (!e.Handled())
		m_rootLayout->OnKeyPressed(e);

	m_keyboardHandlingControl = e.HandlingControl();
	m_keyboardHandlingLayout = e.HandlingLayout();
}
void UI::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	if (m_keyboardHandlingControl != nullptr)
	{
		m_keyboardHandlingControl->OnKeyReleased(e);
	}
	else if (m_keyboardHandlingLayout != nullptr)
	{
		m_keyboardHandlingLayout->OnKeyReleased(e);
	}

	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnKeyReleased(e);
	}

	if (!e.Handled())
		m_rootLayout->OnKeyReleased(e);

	m_keyboardHandlingControl = e.HandlingControl();
	m_keyboardHandlingLayout = e.HandlingLayout();
}
void UI::OnWindowResize(WindowResizeEvent& e) noexcept
{
	m_rootLayout->Resize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
}
void UI::OnMouseMove(MouseMoveEvent& e) noexcept
{
	if (m_panes.size() == 0)
		int iii = 0;

	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseMove(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseMove(e);
	}

	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnMouseMove(e);
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

	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnMouseButtonPressed(e);
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

	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnMouseButtonReleased(e);
	}

	if (!e.Handled())
		m_rootLayout->OnMouseButtonReleased(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}
void UI::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseButtonDoubleClick(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseButtonDoubleClick(e);
	}

	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnMouseButtonDoubleClick(e);
	}

	if (!e.Handled())
		m_rootLayout->OnMouseButtonDoubleClick(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}

void UI::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{
	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnMouseScrolledVertical(e);
	}

	m_rootLayout->OnMouseScrolledVertical(e);
}
void UI::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{
	for (const auto& pane : m_panes)
	{
		if (!e.Handled())
			pane->OnMouseScrolledHorizontal(e);
	}

	m_rootLayout->OnMouseScrolledHorizontal(e);
}


}