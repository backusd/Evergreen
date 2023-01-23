#include "pch.h"
#include "Pane.h"
#include "Controls/Text.h"
#include "Controls/Button.h"
#include "Styles/TextStyle.h"


namespace Evergreen
{
Pane::Pane(std::shared_ptr<DeviceResources> deviceResources,
			UI* ui,
			float top,
			float left,
			float height,
			float width,
			bool resizeable,
			bool relocatable,
			std::unique_ptr<ColorBrush> backgroundBrush,
			std::unique_ptr<ColorBrush> borderBrush,
			float borderWidth,
			bool headerBar,
			std::unique_ptr<ColorBrush> headerBarBrush,
			const std::string& title) :
	m_deviceResources(deviceResources),
	m_title(title),
	m_ui(ui),
	m_top(top),
	m_left(left),
	m_height(height),
	m_width(width),
	m_resizeable(resizeable),
	m_relocatable(relocatable),
	m_headerBar(headerBar),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	m_visible(true),
	m_minimized(false)
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_borderBrush == nullptr && m_headerBar)
		m_headerBarBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Yellow));


	if (m_headerBar)
		InitializeLayoutWithHeaderBar();
	else
		InitializeLayoutWithoutHeaderBar();
}
void Pane::InitializeLayoutWithHeaderBar()
{
	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		m_ui,
		m_top, m_left, m_width, m_height,
		std::move(m_backgroundBrush->Duplicate()),
		m_title + "_Pane_Layout_all"
	);

	m_layout->AddRow({ RowColumnType::FIXED, 20.0f });
	m_layout->AddRow({ RowColumnType::STAR, 1.0f });
	m_layout->AddColumn({ RowColumnType::STAR, 1.0f });
	m_layout->AddColumn({ RowColumnType::FIXED, 20.0f });
	m_layout->AddColumn({ RowColumnType::FIXED, 20.0f });

	RowColumnPosition pos;
	pos.Row = 1;
	pos.Column = 0;
	pos.ColumnSpan = 3;
	m_contentLayout = m_layout->AddSubLayout(pos, m_title + "_Pane_Layout_content");

	// Title ------------------------------------------------------------------
	std::unique_ptr<TextStyle> style = std::make_unique<TextStyle>(
		m_deviceResources,
		"Pane Title TextStyle",
		Evergreen::FontFamily::Arial,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
	);
	std::unique_ptr<SolidColorBrush> titleBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));
	std::wstring title(m_title.begin(), m_title.end());
	m_layout->CreateControl<Text>(m_deviceResources, title, std::move(titleBrush), std::move(style));

	// Minimize Button ----------------------------------------------------------
	RowColumnPosition minimizeButtonPosition;
	minimizeButtonPosition.Row = 0;
	minimizeButtonPosition.Column = 1;

	std::unique_ptr<SolidColorBrush> minimizeBackgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Blue));
	std::unique_ptr<SolidColorBrush> minimizeBorderBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Red));

	Button* minimizeButton = m_layout->CreateControl<Button>(
		minimizeButtonPosition,
		m_deviceResources,
		std::move(minimizeBackgroundBrush),
		std::move(minimizeBorderBrush),
		1.0f
		);
	
	minimizeButton->GetLayout()->AddRow({ RowColumnType::STAR, 1.0f });
	minimizeButton->GetLayout()->AddColumn({ RowColumnType::STAR, 1.0f });

	// Close Button --------------------------------------------------------------
	RowColumnPosition closeButtonPosition;
	closeButtonPosition.Row = 0;
	closeButtonPosition.Column = 2;

	std::unique_ptr<SolidColorBrush> closeBackgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Green));
	std::unique_ptr<SolidColorBrush> closeBorderBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Yellow));

	Button* closeButton = m_layout->CreateControl<Button>(
		closeButtonPosition,
		m_deviceResources,
		std::move(closeBackgroundBrush),
		std::move(closeBorderBrush),
		1.0f
		);

	closeButton->GetLayout()->AddRow({ RowColumnType::STAR, 1.0f });
	closeButton->GetLayout()->AddColumn({ RowColumnType::STAR, 1.0f });
}
void Pane::InitializeLayoutWithoutHeaderBar()
{
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");

	m_layout = std::make_unique<Layout>(
		m_deviceResources,
		m_ui,
		m_top, m_left, m_width, m_height,
		std::move(m_backgroundBrush->Duplicate()),
		m_title + "_Pane_Layout"
	);

	m_contentLayout = m_layout.get();
}

void Pane::Update() noexcept
{
	m_layout->Update();
}
void Pane::Render() const noexcept
{
	m_layout->Render();
}

Row* Pane::AddRow(RowColumnDefinition definition)
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");
	return m_contentLayout->AddRow(definition);
}
Column* Pane::AddColumn(RowColumnDefinition definition)
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");
	return m_contentLayout->AddColumn(definition);
}

}