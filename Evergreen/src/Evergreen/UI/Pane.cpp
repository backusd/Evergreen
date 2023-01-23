#include "pch.h"
#include "Pane.h"
#include "Controls/Text.h"
#include "Controls/Button.h"
#include "Styles/TextStyle.h"
#include "UI.h"


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
			const std::string& title,
			std::unique_ptr<ColorBrush> titleBrush) :
	m_deviceResources(deviceResources),
	m_title(title),
	m_ui(ui),
	m_paneRect(D2D1::RectF(left, top, left + width, top + height)),
	m_resizeable(resizeable),
	m_relocatable(relocatable),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	m_titleBrush(std::move(titleBrush)),
	m_titleLayout(nullptr),
	m_visible(true),
	m_minimized(false),
	m_paneCornerRadiusX(0.0f),
	m_paneCornerRadiusY(0.0f)
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::DarkGray));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	if (m_headerBarBrush == nullptr && headerBar)
		m_headerBarBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));

	if (m_titleBrush == nullptr && headerBar)
		m_titleBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::White));

	if (headerBar)
		InitializeLayoutWithHeaderBar();
	else
		InitializeLayoutWithoutHeaderBar();
}
void Pane::InitializeLayoutWithHeaderBar()
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	
	m_titleLayout = std::make_unique<Layout>(
		m_deviceResources,
		m_ui,
		m_paneRect.top, m_paneRect.left, m_paneRect.right - m_paneRect.left, 20.0f,
		nullptr,
		m_title + "_Pane_Title_Layout"
	);

	m_titleLayout->AddRow({ RowColumnType::FIXED, 20.0f });
	m_titleLayout->AddColumn({ RowColumnType::STAR, 1.0f });	// Title column
	m_titleLayout->AddColumn({ RowColumnType::FIXED, 20.0f });	// Minimize button
	m_titleLayout->AddColumn({ RowColumnType::FIXED, 20.0f });	// Close button

	m_contentLayout = std::make_unique<Layout>(
		m_deviceResources,
		m_ui,
		m_paneRect.top + 20.0f, m_paneRect.left, m_paneRect.right - m_paneRect.left, m_paneRect.bottom - m_paneRect.top - 20.0f,
		nullptr,
		m_title + "_Pane_Content_Layout"
	);

	// Title ------------------------------------------------------------------
	std::unique_ptr<TextStyle> style = std::make_unique<TextStyle>(
		m_deviceResources,
		"Pane Title TextStyle",
		Evergreen::FontFamily::Arial,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
	);
	std::wstring title(m_title.begin(), m_title.end());
	Evergreen::Margin titleMargin{ 5.0f, 0.0f, 0.0f, 0.0f };
	m_titleLayout->CreateControl<Text>(m_deviceResources, title, std::move(m_titleBrush->Duplicate()), std::move(style), titleMargin);

	// Minimize Button ----------------------------------------------------------
	RowColumnPosition minimizeButtonPosition;
	minimizeButtonPosition.Row = 0;
	minimizeButtonPosition.Column = 1;

	std::unique_ptr<SolidColorBrush> minimizeBackgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	Button* minimizeButton = m_titleLayout->CreateControl<Button>(
		minimizeButtonPosition,
		m_deviceResources,
		std::move(minimizeBackgroundBrush)
	);
	
	minimizeButton->GetLayout()->AddRow({ RowColumnType::STAR, 1.0f });
	minimizeButton->GetLayout()->AddColumn({ RowColumnType::STAR, 1.0f });

	std::unique_ptr<TextStyle> minimizeTextStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"Pane Close Button TextStyle",
		Evergreen::FontFamily::Segoe_MDL2_Assets,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
		);
	std::wstring minimizeString = L"";
	minimizeString.push_back(static_cast<wchar_t>(std::stoi(L"0xE738", nullptr, 16)));
	minimizeButton->GetLayout()->CreateControl<Text>(m_deviceResources, minimizeString, std::move(m_titleBrush->Duplicate()), std::move(minimizeTextStyle));

	minimizeButton->SetOnMouseEnteredCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Gray));
			button->BackgroundBrush(std::move(brush));
		}
	);
	minimizeButton->SetOnMouseExitedCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
			button->BackgroundBrush(std::move(brush));
		}
	);	
	minimizeButton->SetOnMouseLButtonDownCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DimGray));
			button->BackgroundBrush(std::move(brush));
		}
	);
	minimizeButton->SetOnMouseLButtonUpCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Gray));
			button->BackgroundBrush(std::move(brush));
		}
	);
	minimizeButton->SetOnClickCallback(
		[this](Control* c, Event& e)
		{
			this->SwitchMinimize();
		}
	);





	// Close Button --------------------------------------------------------------
	RowColumnPosition closeButtonPosition;
	closeButtonPosition.Row = 0;
	closeButtonPosition.Column = 2;

	std::unique_ptr<SolidColorBrush> closeBackgroundBrush = std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	Button* closeButton = m_titleLayout->CreateControl<Button>(
		closeButtonPosition,
		m_deviceResources,
		std::move(closeBackgroundBrush)
		);

	closeButton->GetLayout()->AddRow({ RowColumnType::STAR, 1.0f });
	closeButton->GetLayout()->AddColumn({ RowColumnType::STAR, 1.0f });

	std::unique_ptr<TextStyle> closeTextStyle = std::make_unique<TextStyle>(
		m_deviceResources,
		"Pane Close Button TextStyle",
		Evergreen::FontFamily::Segoe_MDL2_Assets,
		12.0f,
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,
		DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER,
		DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
		DWRITE_WORD_WRAPPING::DWRITE_WORD_WRAPPING_NO_WRAP
		);
	std::wstring closeString = L"";
	closeString.push_back(static_cast<wchar_t>(std::stoi(L"0xE711", nullptr, 16)));
	closeButton->GetLayout()->CreateControl<Text>(m_deviceResources, closeString, std::move(m_titleBrush->Duplicate()), std::move(closeTextStyle));

	closeButton->SetOnMouseEnteredCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Gray));
			button->BackgroundBrush(std::move(brush));
		}
	);
	closeButton->SetOnMouseExitedCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
			button->BackgroundBrush(std::move(brush));
		}
	);
	closeButton->SetOnMouseLButtonDownCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DimGray));
			button->BackgroundBrush(std::move(brush));
		}
	);
	closeButton->SetOnMouseLButtonUpCallback(
		[](Control* c, Event& e)
		{
			Button* button = static_cast<Button*>(c);
			std::unique_ptr<SolidColorBrush> brush = std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Gray));
			button->BackgroundBrush(std::move(brush));
		}
	);
	closeButton->SetOnClickCallback(
		[this](Control* c, Event& e)
		{
			this->GetUI()->RemovePane(this);
			e.ClearHandles(); // Must set handles to nullptr because we don't want the UI to have a dangling pointer to a control that is about to be deleted
		}
	);
}
void Pane::InitializeLayoutWithoutHeaderBar()
{
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");

	m_contentLayout = std::make_unique<Layout>(
		m_deviceResources,
		m_ui,
		m_paneRect.top, m_paneRect.left, m_paneRect.right - m_paneRect.left, m_paneRect.bottom - m_paneRect.top,
		nullptr,
		m_title + "_Pane_Content_Layout"
	);
}

void Pane::Update() noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->Update();

	m_contentLayout->Update();
}
void Pane::Render() const noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");
	
	auto context = m_deviceResources->D2DDeviceContext();

	if (m_titleLayout != nullptr)
	{
		D2D1_RECT_F titleRect = D2D1::RectF(m_paneRect.left, m_paneRect.top, m_paneRect.right, m_paneRect.top + 20.0f);
		D2D1_RECT_F contentRect = D2D1::RectF(m_paneRect.left, m_paneRect.top + 20.0f, m_paneRect.right, m_paneRect.bottom);

		if (m_paneCornerRadiusX > 0.0f && m_paneCornerRadiusY > 0.0f)
		{
			if (m_minimized)
			{
				context->FillRoundedRectangle(
					D2D1::RoundedRect(titleRect, m_paneCornerRadiusX, m_paneCornerRadiusY),
					m_headerBarBrush->Get()
				);
				m_titleLayout->Render();
				
				if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
				{
					context->DrawRoundedRectangle(
						D2D1::RoundedRect(titleRect, m_paneCornerRadiusX, m_paneCornerRadiusY),
						m_borderBrush->Get(), 
						m_borderWidth
					);
				}
			}
			else
			{
				context->PushAxisAlignedClip(titleRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
				context->FillRoundedRectangle(D2D1::RoundedRect(m_paneRect, m_paneCornerRadiusX, m_paneCornerRadiusY), m_headerBarBrush->Get());
				m_titleLayout->Render();
				context->PopAxisAlignedClip();

				context->PushAxisAlignedClip(contentRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
				context->FillRoundedRectangle(D2D1::RoundedRect(m_paneRect, m_paneCornerRadiusX, m_paneCornerRadiusY), m_backgroundBrush->Get());
				m_contentLayout->Render();
				context->PopAxisAlignedClip();

				if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
				{
					context->DrawRoundedRectangle(
						D2D1::RoundedRect(m_paneRect, m_paneCornerRadiusX, m_paneCornerRadiusY),
						m_borderBrush->Get(),
						m_borderWidth
					);
				}
			}
		}
		else
		{
			context->FillRectangle(titleRect, m_headerBarBrush->Get());
			context->FillRectangle(contentRect, m_backgroundBrush->Get());

			m_titleLayout->Render();
			m_contentLayout->Render();

			if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
			{
				context->DrawRectangle(m_paneRect, m_borderBrush->Get(), m_borderWidth);
			}
		}
	}
	else if (!m_minimized)
	{
		// In order to draw the background with rounded corners, we draw the background for the layout manually here
		if (m_paneCornerRadiusX > 0.0f && m_paneCornerRadiusY > 0.0f)
		{
			context->FillRoundedRectangle(D2D1::RoundedRect(m_paneRect, m_paneCornerRadiusX, m_paneCornerRadiusY), m_backgroundBrush->Get());
			m_contentLayout->Render();

			if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
			{
				context->DrawRoundedRectangle(D2D1::RoundedRect(m_paneRect, m_paneCornerRadiusX, m_paneCornerRadiusY), m_borderBrush->Get(), m_borderWidth);
			}
		}
		else
		{
			context->FillRectangle(m_paneRect, m_backgroundBrush->Get());
			m_contentLayout->Render();

			if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
			{
				context->DrawRectangle(m_paneRect, m_borderBrush->Get(), m_borderWidth);
			}
		}
	}
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












void Pane::OnChar(CharEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnChar(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnChar(e);
}
void Pane::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnKeyPressed(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnKeyPressed(e);

}
void Pane::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnKeyReleased(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnKeyReleased(e);
}
void Pane::OnMouseMove(MouseMoveEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnMouseMove(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseMove(e);
}
void Pane::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnMouseScrolledVertical(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseScrolledVertical(e);
}
void Pane::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnMouseScrolledHorizontal(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseScrolledHorizontal(e);
}
void Pane::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnMouseButtonPressed(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseButtonPressed(e);
}
void Pane::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnMouseButtonReleased(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseButtonReleased(e);
}
void Pane::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	if (m_titleLayout != nullptr)
		m_titleLayout->OnMouseButtonDoubleClick(e);

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseButtonDoubleClick(e);
}
}