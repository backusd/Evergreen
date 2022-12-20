#include "pch.h"
#include "UI.h"
#include "JSONLoading/ControlLoaders/TextLoader.h"

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
	m_rootLayout(nullptr)
{
	// Add built-in control loaders
	JSONLoaders::AddControlLoader("Text", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, const json& data, const std::string& controlName) -> Control* { return TextLoader::Load(deviceResources, parentLayout, data, controlName); });
	
	// Add built-in style loaders
	JSONLoaders::AddStyleLoader("TextStyle", [](std::shared_ptr<DeviceResources> deviceResources, const json& data, const std::string& styleName) -> std::shared_ptr<Style> { return TextStyleLoader::Load(deviceResources, data, styleName); });

	LoadDefaultUI();
}

void UI::LoadDefaultUI() noexcept
{
	// TODO: Load a default UI that displays a "No defined layout"
	// Could make this quite elaborate, similar to how ImGui has the example of all controls
	// This could also contain links our website/documentation



	// TEST CODE
	m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()));
	if (std::optional<Row*> row0 = m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f }))
	{
		row0.value()->BottomIsAdjustable(true);
	}
	if (std::optional<Row*> row1 = m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f }))
	{
		row1.value()->TopIsAdjustable(true);
	}

	if (std::optional<Column*> column = m_rootLayout->AddColumn({ RowColumnType::FIXED, 100.0f }))
	{
		column.value()->RightIsAdjustable(true);
	}
	if (std::optional<Column*> column = m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f }))
	{
		column.value()->LeftIsAdjustable(true);
	}

	std::wstring textString = L"Some very long test text";
	std::shared_ptr<TextStyle> defaultStyle = std::make_shared<TextStyle>(m_deviceResources);
	std::shared_ptr<TextStyle> style = std::make_shared<TextStyle>(
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
	std::unique_ptr<SolidColorBrush> b1 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	if (std::optional<Text*> text = m_rootLayout->AddControl<Text>(m_deviceResources, textString, std::move(b1), defaultStyle))
	{
		EG_CORE_TRACE("{}", "Successfully created default text");
	}
	else
	{
		EG_CORE_ERROR("{}", "Failed to create default text");
	}

	RowColumnPosition position;
	position.Row = 1;
	position.Column = 0;
	position.RowSpan = 1;
	position.ColumnSpan = 1;

	std::wstring s = L"Custom Text";
	std::unique_ptr<SolidColorBrush> b2 = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	if (std::optional<Text*> text = m_rootLayout->AddControl<Text>(position, m_deviceResources, s, std::move(b2), style))
	{
		EG_CORE_TRACE("{}", "Successfully created custom text");
	}
	else
	{
		EG_CORE_ERROR("{}", "Failed to create custom text");
	}

	RowColumnPosition position2;
	position2.Row = 0;
	position2.Column = 1;
	position2.RowSpan = 1;
	position2.ColumnSpan = 1;

	if (std::optional<Text*> text = m_rootLayout->AddControl<Text>(position2, m_deviceResources))
	{
		EG_CORE_TRACE("{}", "Successfully created custom text 2");

		text.value()->SetText(L"nooooo...");
	}
	else
	{
		EG_CORE_ERROR("{}", "Failed to create custom text 2");
	}







	
	/*
	if (std::optional<Layout*> sublayoutOpt = m_rootLayout->AddSubLayout({ 0, 0, 1, 1 }, "Test Layout"))
	{
		Layout* sublayout = sublayoutOpt.value();

		sublayout->AddRow({RowColumnType::STAR, 1.0f});
		sublayout->AddRow({ RowColumnType::STAR, 1.0f });
		sublayout->AddRow({ RowColumnType::STAR, 1.0f });

		if (std::optional<Column*> col1 = sublayout->AddColumn({ RowColumnType::STAR, 1.0f }))
		{
			col1.value()->RightIsAdjustable(true);
			col1.value()->MinWidth(10.0f);
		}
		if (std::optional<Column*> col2 = sublayout->AddColumn({ RowColumnType::STAR, 1.0f }))
		{
			col2.value()->LeftIsAdjustable(true);
			col2.value()->MinWidth(10.0f);
		}
	}
	*/
}

void UI::LoadUI(const std::string& fileName) noexcept
{
	// Create a new root layout - this will destroy any layout that previously existed
	m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()), "Root Layout");

	if (!JSONLoaders::LoadUI(m_deviceResources, m_jsonRootDirectory, fileName, m_rootLayout.get()))
	{
		LoadErrorUI();
	}
}

void UI::LoadErrorUI() noexcept
{
	m_rootLayout = std::make_unique<Layout>(0.0f, 0.0f, static_cast<float>(m_window->GetWidth()), static_cast<float>(m_window->GetHeight()));
	m_rootLayout->AddRow({ RowColumnType::STAR, 1.0f });
	m_rootLayout->AddColumn({ RowColumnType::STAR, 1.0f });
	
	std::shared_ptr<TextStyle> style = std::make_shared<TextStyle>(
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
	std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
	
	m_rootLayout->AddControl<Text>(m_deviceResources, text, std::move(brush), style);
}

void UI::Render(DeviceResources* deviceResources) const noexcept
{
	deviceResources->BeginDraw();

	m_rootLayout->Render(deviceResources);




	/*
	//SolidColorBrush brush(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue, 1.0f));


	std::vector<D2D1_GRADIENT_STOP> stops;
	stops.emplace_back(0.0f, D2D1::ColorF(D2D1::ColorF::DarkBlue, 1.0f));
	stops.emplace_back(0.5f, D2D1::ColorF(D2D1::ColorF::Blue, 1.0f));
	stops.emplace_back(1.0f, D2D1::ColorF(D2D1::ColorF::LightBlue, 1.0f));

	GradientBrush brush(
		m_deviceResources,
		stops,
		GRADIENT_AXIS::NWSE
	);

	D2D1_RECT_F rect{ 100.0f, 100.0f, 500.0f, 500.0f };
	brush.SetDrawRegion(rect);
	m_deviceResources->D2DDeviceContext()->FillRectangle(&rect, brush.Get());

	// -------------

	std::vector<D2D1_GRADIENT_STOP> stops2;
	stops2.emplace_back(0.0f, D2D1::ColorF(D2D1::ColorF::Red, 1.0f));
	stops2.emplace_back(0.5f, D2D1::ColorF(D2D1::ColorF::Blue, 1.0f));
	stops2.emplace_back(1.0f, D2D1::ColorF(D2D1::ColorF::Green, 1.0f));

	RadialBrush radialBrush(
		m_deviceResources, 
		stops2
	);

	D2D1_ELLIPSE ellipse = D2D1::Ellipse({ 600.0f, 600.0f }, 75.0f, 75.0f);
	D2D1_RECT_F rect22 = D2D1::RectF(525.0f, 525.0f, 675.0f, 675.0f);
	radialBrush.SetDrawRegion(rect22);
	m_deviceResources->D2DDeviceContext()->FillEllipse(ellipse, radialBrush.Get());
	// -----------------

	D2D1_RECT_F rect2;
	rect2.bottom = 450.0f;
	rect2.left = 750.0f;
	rect2.top = 200.0f;
	rect2.right = 1050.0f;

	BitmapBrush bmbrush(m_deviceResources, L"evergreen-image-1.jpg");
	bmbrush.TransformToRect(rect2, TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_UNDERFILL_RECT);
	bmbrush.ExtendMode(D2D1_EXTEND_MODE_WRAP);

	m_deviceResources->D2DDeviceContext()->FillRectangle(
		&rect2,
		bmbrush.Get()
	);
	*/



	deviceResources->EndDraw();
}

void UI::OnWindowResize(WindowResizeEvent& e) noexcept
{
	m_rootLayout->OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
}

void UI::OnMouseMove(MouseMoveEvent& e) noexcept
{
	m_rootLayout->OnMouseMove(e);
}
void UI::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	m_rootLayout->OnMouseButtonPressed(e);
}
void UI::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	m_rootLayout->OnMouseButtonReleased(e);
}


}