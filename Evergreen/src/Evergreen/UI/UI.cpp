#include "pch.h"
#include "UI.h"
#include "JSONLoading/ControlLoaders/TextLoader.h"
#include "JSONLoading/ControlLoaders/ButtonLoader.h"
#include "JSONLoading/ControlLoaders/RoundedButtonLoader.h"
#include "JSONLoading/ControlLoaders/ScrollableLayoutLoader.h"
#include "JSONLoading/ControlLoaders/TextInputLoader.h"
#include "JSONLoading/ControlLoaders/PaneLoader.h"
#include "JSONLoading/ControlLoaders/SliderFloatLoader.h"
#include "JSONLoading/ControlLoaders/SliderIntLoader.h"
#include "JSONLoading/ControlLoaders/RadioButtonLoader.h"
#include "JSONLoading/ControlLoaders/ViewportLoader.h"

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
	m_keyboardHandlingLayout(nullptr),
	m_mouseLButtonDown(false),
	m_mouseMButtonDown(false),
	m_mouseRButtonDown(false),
	m_mouseX1ButtonDown(false),
	m_mouseX2ButtonDown(false)
{
	// Add built-in control loaders
	JSONLoaders::AddControlLoader("Text", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return TextLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("Button", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return ButtonLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("RoundedButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return RoundedButtonLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("ScrollableLayout", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return ScrollableLayoutLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("TextInput", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return TextInputLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("Pane", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return PaneLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("SliderFloat", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return SliderFloatLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("SliderInt", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return SliderIntLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("RadioButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return RadioButtonLoader::Load(deviceResources, parentLayout, data, controlName); });
	JSONLoaders::AddControlLoader("Viewport", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName) -> Control* { return ViewportLoader::Load(deviceResources, parentLayout, data, controlName); });

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
			TextInput* textInput = this->GetControlByName<TextInput>("TextInputControl");

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

	std::unique_ptr<Pane> p = std::make_unique<Pane>(
		m_deviceResources,
		this, // UI*
		150.0f, // top
		150.0f, // left
		500.0f, // height
		700.0f, // width
		true, // resizable 
		true, // relocatable
		nullptr, // background brush
		nullptr, // border brush
		1.0f, // border width
		true, // includeTitleBar
		nullptr, // TitleBarBrush
		20.0f // TitleBar height
		);
	Pane* pane = AddPane(std::move(p), "Test Pane");
	//Pane* pane = GetPane("Test Pane");

	pane->SetCornerRadius(8.0f);

	Layout* titleLayout = pane->GetTitleBarLayout();
	titleLayout->AddRow({ RowColumnType::STAR, 1.0f });
	titleLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	pane->ClearTitleBarLayoutAndAddTitle("Some title bitch");

	pane->SetOnMouseEnteredTitleBarCallback(
		[](Control* control, Event& e)
		{
			Pane* pane = static_cast<Pane*>(control);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue));
			pane->SetTitleBarBrush(std::move(brush));
		}
	);
	pane->SetOnMouseExitedTitleBarCallback(
		[](Control* control, Event& e)
		{
			Pane* pane = static_cast<Pane*>(control);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));
			pane->SetTitleBarBrush(std::move(brush));
		}
	);
	pane->SetOnMouseEnteredContentRegionCallback(
		[](Control* control, Event& e)
		{
			Pane* pane = static_cast<Pane*>(control);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
			pane->SetBackgroundBrush(std::move(brush));
		}
	);
	pane->SetOnMouseExitedContentRegionCallback(
		[](Control* control, Event& e)
		{
			Pane* pane = static_cast<Pane*>(control);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkGray));
			pane->SetBackgroundBrush(std::move(brush));
		}
	);
	pane->SetOnMouseMovedCallback(
		[](Control* control, Event& _e)
		{
			Pane* pane = static_cast<Pane*>(control);
			MouseMoveEvent& e = dynamic_cast<MouseMoveEvent&>(_e);

			std::string newTitle = std::format("x: {}, y: {}", e.GetX(), e.GetY());
			pane->ClearTitleBarLayoutAndAddTitle(newTitle);			
		}
	);

	pane->AddRow({ RowColumnType::STAR, 1.0f });
	pane->AddRow({ RowColumnType::STAR, 1.0f });
	pane->AddColumn({ RowColumnType::STAR, 1.0f });
	pane->AddColumn({ RowColumnType::STAR, 1.0f });
	
	RowColumnPosition paneContentPosition;
	paneContentPosition.Row = 0;
	paneContentPosition.Column = 0;

	Text* t1 = pane->CreateControl<Text>(paneContentPosition, m_deviceResources, L"Top Left");

	paneContentPosition.Column = 1;
	Text* t2 = pane->CreateControl<Text>(paneContentPosition, m_deviceResources, L"Top Right");

	paneContentPosition.Row = 1;
	paneContentPosition.Column = 0;
	Text* t3 = pane->CreateControl<Text>(paneContentPosition, m_deviceResources, L"Bottom Left");

	paneContentPosition.Column = 1;
	Layout* paneSublayout = pane->AddSubLayout(paneContentPosition);
	paneSublayout->AddRow({ RowColumnType::STAR, 1.0f });
	paneSublayout->AddRow({ RowColumnType::STAR, 1.0f });
	paneSublayout->AddColumn({ RowColumnType::STAR, 1.0f });
	paneSublayout->AddColumn({ RowColumnType::STAR, 1.0f });

	paneContentPosition.Row = 0;
	paneContentPosition.Column = 0;
	t1 = paneSublayout->CreateControl<Text>(paneContentPosition, m_deviceResources, L"Top Left");

	paneContentPosition.Column = 1;
	t2 = paneSublayout->CreateControl<Text>(paneContentPosition, m_deviceResources, L"Top Right");

	paneContentPosition.Row = 1;
	paneContentPosition.Column = 0;
	t3 = paneSublayout->CreateControl<Text>(paneContentPosition, m_deviceResources, L"Bottom Left");

	paneContentPosition.Column = 1;
	t3 = paneSublayout->CreateControl<Text>(paneContentPosition, m_deviceResources, L"Bottom Right");


	// Slider ======================================================================================
	RowColumnPosition sliderPosition;
	sliderPosition.Row = 1;
	sliderPosition.Column = 1;

	//std::unique_ptr<SolidColorBrush> tiBackgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::WhiteSmoke));

	Evergreen::Margin sliderMargin = { 10.0f, 0.0f, 10.0f, 0.0f };

	SliderFloat* slider = sublayout->CreateControl<SliderFloat>(
		sliderPosition,
		m_deviceResources,
		0.0f, // min
		10.0f, // max
		5.0f, // initial
		sliderMargin
		);
	slider->Name("Slider Float #1");

	sliderPosition.Row = 2;
	/*
	Slider<int>* slider2 = sublayout->CreateControl<Slider<int>>(
		sliderPosition,
		m_deviceResources,
		0, // min
		10, // max
		5, // initial
		sliderMargin
	);
	slider2->Name("Slider2");
	slider2->SetValueFormatString(L"{}");
	*/


	RowColumnPosition sliderCheckPosition;
	sliderCheckPosition.Row = 2;
	sliderCheckPosition.Column = 1;
	Text* sliderCheck = sublayout->CreateControl<Text>(sliderCheckPosition, m_deviceResources, L"Slider Check");

	slider->SetOnValueChangedCallback(
		[sliderCheck](Control* c, Event& e)
		{
			SliderFloat* slider = static_cast<SliderFloat*>(c);
			SliderFloatValueChangedEvent& evnt = dynamic_cast<SliderFloatValueChangedEvent&>(e);

			sliderCheck->SetText(std::format(L"{}", evnt.GetValue()));
		}
	);

	slider->SetOnMouseEnteredCircleCallback(
		[sliderCheck](Control* c, Event& e)
		{
			SliderFloat* slider = static_cast<SliderFloat*>(c);

			std::unique_ptr<Evergreen::ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Red));
			slider->SetLineBrushLeft(std::move(brush));
		}
	);
	slider->SetOnMouseExitedCircleCallback(
		[sliderCheck](Control* c, Event& e)
		{
			SliderFloat* slider = static_cast<SliderFloat*>(c);

			std::unique_ptr<Evergreen::ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
			slider->SetLineBrushLeft(std::move(brush));
		}
	);

	slider->SetOnBeginDraggingCallback(
		[sliderCheck](Control* c, Event& e)
		{
			SliderFloat* slider = static_cast<SliderFloat*>(c);
			slider->SetFillLineOnRightSide(true);
		}
	);
	slider->SetOnStoppedDraggingCallback(
		[sliderCheck](Control* c, Event& e)
		{
			SliderFloat* slider = static_cast<SliderFloat*>(c);
			slider->SetFillLineOnRightSide(false);
		}
	);

	// RadioButton =====================================================================
	RowColumnPosition rbPosition;
	rbPosition.Row = 2;
	rbPosition.Column = 1;

	RadioButton* rb = sublayout->CreateControl<RadioButton>(
		rbPosition,
		m_deviceResources, 
		true, // isChecked
		6.0f, // outerRadius
		3.0f, // innerRadius
		nullptr, // outerBrush
		nullptr // innerBrush
	);

	rb->SetOnMouseEnteredCallback(
		[](RadioButton* rb, MouseMoveEvent& e)
		{
			EG_CORE_INFO("ENTERED: {}, {}", e.GetX(), e.GetY());
		}
	);
	rb->SetOnMouseExitedCallback(
		[](RadioButton* rb, MouseMoveEvent& e)
		{
			EG_CORE_INFO("EXITED: {}, {}", e.GetX(), e.GetY());
		}
	);
	rb->SetOnMouseMovedCallback(
		[](RadioButton* rb, MouseMoveEvent& e)
		{
			EG_CORE_INFO("MOVED: {}, {}", e.GetX(), e.GetY());
		}
	);
	rb->SetOnMouseLButtonDownCallback(
		[](RadioButton* rb, MouseButtonPressedEvent& e)
		{
			EG_CORE_INFO("LButton PRESSED: {}, {}", e.GetX(), e.GetY());
		}
	);
	rb->SetOnMouseLButtonUpCallback(
		[](RadioButton* rb, MouseButtonReleasedEvent& e)
		{
			EG_CORE_INFO("LButton RELEASED: {}, {}", e.GetX(), e.GetY());
		}
	);
	rb->SetOnIsCheckedChanged(
		[](RadioButton* rb, RadioButtonIsCheckedChangedEvent& e)
		{
			EG_CORE_INFO("Check Changed: {}", e.IsChecked());
		}
	);

	// =====================================================================================
	// Viewport
	RowColumnPosition vpPosition;
	vpPosition.Row = 1;
	vpPosition.Column = 1;

	Viewport* vp = m_rootLayout->CreateControl<Viewport>(
		vpPosition,
		m_deviceResources
	);
	Layout* vpLayout = vp->GetLayout();
	vpLayout->AddRow({ RowColumnType::STAR, 1.0f });
	vpLayout->AddColumn({ RowColumnType::STAR, 1.0f });

	Text* vpText = vpLayout->CreateControl<Text>(m_deviceResources, L"Viewport");
}

void UI::LoadUI(const std::string& fileName) noexcept
{
	// Clear any panes that were previously created
	m_panes.clear();

	// Create a new root layout - this will destroy any layout that previously existed
	m_rootLayout = std::make_unique<Layout>(
		m_deviceResources, 
		this,
		0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()),
		nullptr,
		"Root Layout");

	if (!JSONLoaders::LoadUI(m_deviceResources, m_jsonRootDirectory, fileName, m_rootLayout.get()))
	{
		// When loading the UI fails, we must also clean up all panes
		m_panes.clear();

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

void UI::Update(const Timer& timer) noexcept
{
	m_rootLayout->Update(timer);
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

Pane* UI::AddPane(std::unique_ptr<Pane> pane, const std::string& name) noexcept
{
	EG_CORE_ASSERT(m_panesMap.find(name) == m_panesMap.end(), std::format("Pane with name '{}' already exists", name));
	EG_CORE_ASSERT(name.size() > 0, "Pane name cannot be empty");
	EG_CORE_ASSERT(pane != nullptr, "Input pane should not be nullptr");

	m_panes.push_back(std::move(pane));
	m_panesMap[name] = m_panes.back().get();
	return m_panes.back().get();
}
Pane* UI::GetPane(const std::string& name) noexcept
{
	EG_CORE_ASSERT(name.size() > 0, "Pane search name should not be empty");
	EG_CORE_ASSERT(m_panesMap.find(name) != m_panesMap.end(), std::format("Pane with name '{}' does not exist", name));

	return m_panesMap[name];
}
void UI::RemovePane(const std::string& name) noexcept
{
	EG_CORE_ASSERT(name.size() > 0, "Pane search name should not be empty");
	EG_CORE_ASSERT(m_panesMap.find(name) != m_panesMap.end(), std::format("Pane with name '{}' does not exist", name));

	// We must erase from vector last because it will destroy the unique_ptr for the Pane, but if we just erase from the map
	// first, then we can't get back the Pane*. So get the Pane* first, then erase it from the map and then remove it from the vector
	Pane* p = m_panesMap[name];
	m_panesMap.erase(name);
	RemovePaneFromVector(p);
}
void UI::RemovePane(Pane* pane) noexcept
{
	EG_CORE_ASSERT(pane != nullptr, "Input parameter should not be nullptr");

	std::string name = "";
	for (auto it = m_panesMap.begin(); it != m_panesMap.end(); ++it)
	{
		if (it->second == pane)
		{
			name = it->first;
			break;
		}
	}

	EG_CORE_ASSERT(name.size() > 0, "Could not remove Pane from m_panesMap because it does not exist.");

	m_panesMap.erase(name);
	RemovePaneFromVector(pane);
}
void UI::RemovePaneFromVector(Pane* pane) noexcept
{
	EG_CORE_ASSERT(pane != nullptr, "Input pane should not be nullptr");

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
void UI::BringPaneToForeground(const std::string& name) noexcept
{
	EG_CORE_ASSERT(name.size() > 0, "Pane search name should not be empty");
	EG_CORE_ASSERT(m_panesMap.find(name) != m_panesMap.end(), std::format("Pane with name '{}' does not exist", name));

	BringPaneToForeground(m_panesMap[name]);
}
void UI::BringPaneToForeground(Pane* pane) noexcept
{
	EG_CORE_ASSERT(pane != nullptr, "Input pane should not be nullptr");

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
void UI::ClearHandlingControlAndLayout() noexcept
{
	m_mouseHandlingControl = nullptr;
	m_mouseHandlingLayout = nullptr;
	m_keyboardHandlingControl = nullptr;
	m_keyboardHandlingLayout = nullptr;
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
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnChar(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
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

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnKeyPressed(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
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

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnKeyReleased(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
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
	// If no mouse buttons are in use, we need to first test if the mouse is now over a pane
	// Because even if the mouseHandlingControl is not nullptr, we might be over a pane now that resides on top
	// of the mouseHandlingControl
	if (m_panes.size() > 0 && !(m_mouseLButtonDown || m_mouseMButtonDown || m_mouseRButtonDown || m_mouseX1ButtonDown || m_mouseX2ButtonDown))
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnMouseMove(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}

		if (!e.Handled())
		{
			if (m_mouseHandlingControl != nullptr)
			{
				m_mouseHandlingControl->OnMouseMove(e);
			}
			else if (m_mouseHandlingLayout != nullptr)
			{
				m_mouseHandlingLayout->OnMouseMove(e);
			}
		}

		if (!e.Handled())
			m_rootLayout->OnMouseMove(e);

		m_mouseHandlingControl = e.HandlingControl();
		m_mouseHandlingLayout = e.HandlingLayout();
		return;
	}

	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseMove(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseMove(e);
	}

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnMouseMove(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
	}

	if (!e.Handled())
		m_rootLayout->OnMouseMove(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}
void UI::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	switch (e.GetMouseButton())
	{
	case Evergreen::MOUSE_BUTTON::EG_LBUTTON: m_mouseLButtonDown = true; break;
	case Evergreen::MOUSE_BUTTON::EG_MBUTTON: m_mouseMButtonDown = true; break;
	case Evergreen::MOUSE_BUTTON::EG_RBUTTON: m_mouseRButtonDown = true; break;
	case Evergreen::MOUSE_BUTTON::EG_XBUTTON1: m_mouseX1ButtonDown = true; break;
	case Evergreen::MOUSE_BUTTON::EG_XBUTTON2: m_mouseX2ButtonDown = true; break;
	}

	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseButtonPressed(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseButtonPressed(e);
	}

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnMouseButtonPressed(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
	}

	if (!e.Handled())
		m_rootLayout->OnMouseButtonPressed(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}
void UI::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	switch (e.GetMouseButton())
	{
	case Evergreen::MOUSE_BUTTON::EG_LBUTTON: m_mouseLButtonDown = false; break;
	case Evergreen::MOUSE_BUTTON::EG_MBUTTON: m_mouseMButtonDown = false; break;
	case Evergreen::MOUSE_BUTTON::EG_RBUTTON: m_mouseRButtonDown = false; break;
	case Evergreen::MOUSE_BUTTON::EG_XBUTTON1: m_mouseX1ButtonDown = false; break;
	case Evergreen::MOUSE_BUTTON::EG_XBUTTON2: m_mouseX2ButtonDown = false; break;
	}

	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseButtonReleased(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseButtonReleased(e);
	}

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnMouseButtonReleased(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
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

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnMouseButtonDoubleClick(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
	}

	if (!e.Handled())
		m_rootLayout->OnMouseButtonDoubleClick(e);

	m_mouseHandlingControl = e.HandlingControl();
	m_mouseHandlingLayout = e.HandlingLayout();
}

void UI::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{
	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseScrolledVertical(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseScrolledVertical(e);
	}

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnMouseScrolledVertical(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
	}

	m_rootLayout->OnMouseScrolledVertical(e);
}
void UI::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{
	if (m_mouseHandlingControl != nullptr)
	{
		m_mouseHandlingControl->OnMouseScrolledHorizontal(e);
	}
	else if (m_mouseHandlingLayout != nullptr)
	{
		m_mouseHandlingLayout->OnMouseScrolledHorizontal(e);
	}

	if (!e.Handled())
	{
		const size_t initialPaneCount = m_panes.size();
		for (const auto& pane : m_panes)
		{
			if (!e.Handled())
				pane->OnMouseScrolledHorizontal(e);

			// Any time we iterate over the panes, it is possible that a Pane is destroyed. When this is the case,
			// we cannot safely continue to iterate over the panes. Even if the control says that it is Handled, the handling
			// control may be within the Pane, so we cannot safely keep it as the handling control. Best thing to do here is just
			// null out the handling pointers and return
			if (m_panes.size() != initialPaneCount)
			{
				m_mouseHandlingControl = nullptr;
				m_mouseHandlingLayout = nullptr;
				m_keyboardHandlingControl = nullptr;
				m_keyboardHandlingLayout = nullptr;
				return;
			}
		}
	}

	m_rootLayout->OnMouseScrolledHorizontal(e);
}

}