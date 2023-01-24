#include "pch.h"
#include "Pane.h"
#include "Controls/Text.h"
#include "Controls/Button.h"
#include "Styles/TextStyle.h"
#include "UI.h"

using Microsoft::WRL::ComPtr;


namespace Evergreen
{
const float Pane::edgeSensitivity = 3.0f;

Pane::Pane(std::shared_ptr<DeviceResources> deviceResources,
			UI* ui,
			float top,
			float left,
			float height,
			float width,
			const std::string& title,
			bool resizeable,
			bool relocatable,
			std::unique_ptr<ColorBrush> backgroundBrush,
			std::unique_ptr<ColorBrush> borderBrush,
			float borderWidth,
			bool headerBar,
			std::unique_ptr<ColorBrush> headerBarBrush,
			std::unique_ptr<ColorBrush> titleBrush,
			float titleBarHeight) :
	Control(deviceResources, ui, D2D1::RectF(left, top, left + width, top + height)),
	m_title(title),
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
	m_paneCornerRadiusY(0.0f),
	m_titleBarHeight(titleBarHeight),
	m_lastMouseX(0.0f),
	m_lastMouseY(0.0f),
	m_minPaneWidth(100.0f), 
	m_minPaneHeight(100.0f),
	m_mouseTitleBarState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseRightEdgeState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseLeftEdgeState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseTopEdgeState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseBottomEdgeState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseBottomRightCornerState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseBottomLeftCornerState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseTopRightCornerState(MouseOverDraggableAreaState::NOT_OVER),
	m_mouseTopLeftCornerState(MouseOverDraggableAreaState::NOT_OVER)
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
		m_allowedRegion.top, m_allowedRegion.left, m_allowedRegion.right - m_allowedRegion.left, m_titleBarHeight,
		nullptr,
		m_title + "_Pane_Title_Layout"
	);

	m_titleLayout->AddRow({ RowColumnType::FIXED, m_titleBarHeight });
	m_titleLayout->AddColumn({ RowColumnType::STAR, 1.0f });	// Title column
	m_titleLayout->AddColumn({ RowColumnType::FIXED, 20.0f });	// Minimize button
	m_titleLayout->AddColumn({ RowColumnType::FIXED, 20.0f });	// Close button

	m_contentLayout = std::make_unique<Layout>(
		m_deviceResources,
		m_ui,
		m_allowedRegion.top + m_titleBarHeight, m_allowedRegion.left, m_allowedRegion.right - m_allowedRegion.left, m_allowedRegion.bottom - m_allowedRegion.top - m_titleBarHeight,
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
	Evergreen::Margin titleMargin{ 10.0f, 0.0f, 0.0f, 0.0f };
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
			// Set cursor here because it may be entering from outside the pane and be a double arrow
			Window::SetCursor(Cursor::ARROW);

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
		m_allowedRegion.top, m_allowedRegion.left, m_allowedRegion.right - m_allowedRegion.left, m_allowedRegion.bottom - m_allowedRegion.top,
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
		D2D1_RECT_F titleRect = TitleRect();
		D2D1_RECT_F contentRect = ContentRect();

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
				context->FillRoundedRectangle(D2D1::RoundedRect(m_allowedRegion, m_paneCornerRadiusX, m_paneCornerRadiusY), m_headerBarBrush->Get());
				m_titleLayout->Render();
				context->PopAxisAlignedClip();

				context->PushAxisAlignedClip(contentRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
				context->FillRoundedRectangle(D2D1::RoundedRect(m_allowedRegion, m_paneCornerRadiusX, m_paneCornerRadiusY), m_backgroundBrush->Get());
				m_contentLayout->Render();
				context->PopAxisAlignedClip();

				if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
				{
					context->DrawRoundedRectangle(
						D2D1::RoundedRect(m_allowedRegion, m_paneCornerRadiusX, m_paneCornerRadiusY),
						m_borderBrush->Get(),
						m_borderWidth
					);
				}
			}
		}
		else
		{
			context->FillRectangle(titleRect, m_headerBarBrush->Get());
			m_titleLayout->Render();

			if (!m_minimized)
			{
				context->FillRectangle(contentRect, m_backgroundBrush->Get());
				m_contentLayout->Render();
			}

			if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
			{
				if (m_minimized)
					context->DrawRectangle(titleRect, m_borderBrush->Get(), m_borderWidth);
				else
					context->DrawRectangle(m_allowedRegion, m_borderBrush->Get(), m_borderWidth);
			}
		}
	}
	else if (!m_minimized)
	{
		// In order to draw the background with rounded corners, we draw the background for the layout manually here
		if (m_paneCornerRadiusX > 0.0f && m_paneCornerRadiusY > 0.0f)
		{
			context->FillRoundedRectangle(D2D1::RoundedRect(m_allowedRegion, m_paneCornerRadiusX, m_paneCornerRadiusY), m_backgroundBrush->Get());
			m_contentLayout->Render();

			if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
			{
				context->DrawRoundedRectangle(D2D1::RoundedRect(m_allowedRegion, m_paneCornerRadiusX, m_paneCornerRadiusY), m_borderBrush->Get(), m_borderWidth);
			}
		}
		else
		{
			context->FillRectangle(m_allowedRegion, m_backgroundBrush->Get());
			m_contentLayout->Render();

			if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
			{
				context->DrawRectangle(m_allowedRegion, m_borderBrush->Get(), m_borderWidth);
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





void Pane::PaneChanged() noexcept
{
	D2D1_RECT_F titleRect = TitleRect();
	D2D1_RECT_F contentRect = ContentRect();

	m_titleLayout->Resize(titleRect);
	m_contentLayout->Resize(contentRect);

	m_headerBarBrush->SetDrawRegion(titleRect);
	m_titleBrush->SetDrawRegion(titleRect);
	m_backgroundBrush->SetDrawRegion(contentRect);
	m_borderBrush->SetDrawRegion(contentRect);
}
void Pane::OnMarginChanged()
{
	PaneChanged();
}
void Pane::OnAllowedRegionChanged()
{
	PaneChanged();
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

	// Check if there is even a title bar
	if (m_titleLayout != nullptr)
	{
		// First check if we are dragging before passing to layouts
		if (m_mouseTopRightCornerState == MouseOverDraggableAreaState::DRAGGING)
		{
			float deltaX = 0.0f;
			float deltaY = 0.0f;
			
			if (e.GetX() >= m_allowedRegion.left + m_minPaneWidth)
			{
				deltaX = e.GetX() - m_lastMouseX;
				m_lastMouseX = e.GetX();
			}
			if (e.GetY() <= m_allowedRegion.bottom - m_minPaneHeight)
			{
				deltaY = e.GetY() - m_lastMouseY;
				m_lastMouseY = e.GetY();
			}

			if (deltaX != 0.0f || deltaY != 0.0f)
			{
				AllowedRegion(
					m_allowedRegion.left,
					std::min(m_allowedRegion.bottom - m_minPaneHeight, m_allowedRegion.top + deltaY),
					std::max(m_allowedRegion.left + m_minPaneWidth, m_allowedRegion.right + deltaX),
					m_allowedRegion.bottom
				);
			}

			e.Handled(this);
			return;
		}

		if (m_mouseTopLeftCornerState == MouseOverDraggableAreaState::DRAGGING)
		{
			float deltaX = 0.0f;
			float deltaY = 0.0f;

			if (e.GetX() <= m_allowedRegion.right - m_minPaneWidth)
			{
				deltaX = e.GetX() - m_lastMouseX;
				m_lastMouseX = e.GetX();
			}
			if (e.GetY() <= m_allowedRegion.bottom - m_minPaneHeight)
			{
				deltaY = e.GetY() - m_lastMouseY;
				m_lastMouseY = e.GetY();
			}

			if (deltaX != 0.0f || deltaY != 0.0f)
			{
				AllowedRegion(
					std::min(m_allowedRegion.right - m_minPaneWidth, m_allowedRegion.left + deltaX),
					std::min(m_allowedRegion.bottom - m_minPaneHeight, m_allowedRegion.top + deltaY),
					m_allowedRegion.right,
					m_allowedRegion.bottom
				);
			}

			e.Handled(this);
			return;
		}

		if (m_mouseBottomRightCornerState == MouseOverDraggableAreaState::DRAGGING)
		{
			float deltaX = 0.0f;
			float deltaY = 0.0f;

			if (e.GetX() >= m_allowedRegion.left + m_minPaneWidth)
			{
				deltaX = e.GetX() - m_lastMouseX;
				m_lastMouseX = e.GetX();
			}
			if (e.GetY() >= m_allowedRegion.top + m_minPaneHeight)
			{
				deltaY = e.GetY() - m_lastMouseY;
				m_lastMouseY = e.GetY();
			}

			if (deltaX != 0.0f || deltaY != 0.0f)
			{
				AllowedRegion(
					m_allowedRegion.left,
					m_allowedRegion.top,
					std::max(m_allowedRegion.left + m_minPaneWidth, m_allowedRegion.right + deltaX),
					std::max(m_allowedRegion.top + m_minPaneHeight, m_allowedRegion.bottom + deltaY)
				);
			}

			e.Handled(this);
			return;
		}

		if (m_mouseBottomLeftCornerState == MouseOverDraggableAreaState::DRAGGING)
		{
			float deltaX = 0.0f;
			float deltaY = 0.0f;

			if (e.GetX() <= m_allowedRegion.right - m_minPaneWidth)
			{
				deltaX = e.GetX() - m_lastMouseX;
				m_lastMouseX = e.GetX();
			}
			if (e.GetY() >= m_allowedRegion.top + m_minPaneHeight)
			{
				deltaY = e.GetY() - m_lastMouseY;
				m_lastMouseY = e.GetY();
			}

			if (deltaX != 0.0f || deltaY != 0.0f)
			{
				AllowedRegion(
					std::min(m_allowedRegion.right - m_minPaneWidth, m_allowedRegion.left + deltaX),
					m_allowedRegion.top,
					m_allowedRegion.right,
					std::max(m_allowedRegion.top + m_minPaneHeight, m_allowedRegion.bottom + deltaY)
				);
			}

			e.Handled(this);
			return;
		}

		if (m_mouseRightEdgeState == MouseOverDraggableAreaState::DRAGGING)
		{
			// We want to disable the case where the user selected the right edge, drags it to the left and the min width is reached, and they
			// continue dragging to the left and nothing seems to happen. Then if they move their cursor to the right, even though the cursor
			// is no longer over the edge, it will move the edge to the right. To stop this, only compute a deltaX if the mouse is right of where
			// the edge would be at minimum width
			if (e.GetX() < m_allowedRegion.left + m_minPaneWidth)
			{
				e.Handled(this);
				return;
			}

			float deltaX = e.GetX() - m_lastMouseX;
			AllowedRegionRight(std::max(m_allowedRegion.left + m_minPaneWidth, m_allowedRegion.right + deltaX));

			m_lastMouseX = e.GetX();
			e.Handled(this);
			return;
		}

		if (m_mouseLeftEdgeState == MouseOverDraggableAreaState::DRAGGING)
		{
			// We want to disable the case where the user selected the left edge, drags it to the right and the min width is reached, and they
			// continue dragging to the right and nothing seems to happen. Then if they move their cursor to the left, even though the cursor
			// is no longer over the edge, it will move the edge to the left. To stop this, only compute a deltaX if the mouse is left of where
			// the edge would be at minimum width
			if (e.GetX() > m_allowedRegion.right - m_minPaneWidth)
			{
				e.Handled(this);
				return;
			}

			float deltaX = e.GetX() - m_lastMouseX;
			AllowedRegionLeft(std::min(m_allowedRegion.right - m_minPaneWidth, m_allowedRegion.left + deltaX));

			m_lastMouseX = e.GetX();
			e.Handled(this);
			return;
		}

		if (m_mouseTopEdgeState == MouseOverDraggableAreaState::DRAGGING)
		{
			// We want to disable the case where the user selected the top edge, drags it down and the min height is reached, and they
			// continue dragging down and nothing seems to happen. Then if they move their cursor up, even though the cursor
			// is no longer over the edge, it will move the edge upward. To stop this, only compute a deltaY if the mouse is above where
			// the edge would be at minimum height
			if (e.GetY() > m_allowedRegion.bottom - m_minPaneHeight)
			{
				e.Handled(this);
				return;
			}

			float deltaY = e.GetY() - m_lastMouseY;
			AllowedRegionTop(std::min(m_allowedRegion.bottom - m_minPaneHeight, m_allowedRegion.top + deltaY));

			m_lastMouseY = e.GetY();
			e.Handled(this);
			return;
		}

		if (m_mouseBottomEdgeState == MouseOverDraggableAreaState::DRAGGING)
		{
			// We want to disable the case where the user selected the bottom edge, drags it up and the min height is reached, and they
			// continue dragging up and nothing seems to happen. Then if they move their cursor down, even though the cursor
			// is no longer over the edge, it will move the edge downward. To stop this, only compute a deltaY if the mouse is below where
			// the edge would be at minimum height
			if (e.GetY() < m_allowedRegion.top + m_minPaneHeight)
			{
				e.Handled(this);
				return;
			}

			float deltaY = e.GetY() - m_lastMouseY;
			AllowedRegionBottom(std::max(m_allowedRegion.top + m_minPaneHeight, m_allowedRegion.bottom + deltaY));

			m_lastMouseY = e.GetY();
			e.Handled(this);
			return;
		}

		if (m_mouseTitleBarState == MouseOverDraggableAreaState::DRAGGING)
		{
			float deltaX = e.GetX() - m_lastMouseX;
			float deltaY = e.GetY() - m_lastMouseY;
			AllowedRegion(m_allowedRegion.left + deltaX, m_allowedRegion.top + deltaY, m_allowedRegion.right + deltaX, m_allowedRegion.bottom + deltaY);

			m_lastMouseX = e.GetX();
			m_lastMouseY = e.GetY();
			e.Handled(this);
			return;
		}

		// Allow the title bar to handle the event first
		if (m_titleLayout != nullptr)
		{
			m_titleLayout->OnMouseMove(e);
			if (e.Handled())
				return;
		}

		// Check for other mouse state changes
		//
		// Top Right Corner
		bool mouseIsOverTopRightCorner = RectContainsPoint(TopRightCornerRect(), e.GetX(), e.GetY());
		if (m_mouseTopRightCornerState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverTopRightCorner)
			{
				m_mouseTopRightCornerState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_NESW);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseTopRightCornerState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverTopRightCorner)
			{
				m_mouseTopRightCornerState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Top Left Corner
		bool mouseIsOverTopLeftCorner = RectContainsPoint(TopLeftCornerRect(), e.GetX(), e.GetY());
		if (m_mouseTopLeftCornerState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverTopLeftCorner)
			{
				m_mouseTopLeftCornerState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_NWSE);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseTopLeftCornerState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverTopLeftCorner)
			{
				m_mouseTopLeftCornerState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Bottom Right Corner
		bool mouseIsOverBottomRightCorner = RectContainsPoint(BottomRightCornerRect(), e.GetX(), e.GetY());
		if (m_mouseBottomRightCornerState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverBottomRightCorner)
			{
				m_mouseBottomRightCornerState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_NWSE);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseBottomRightCornerState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverBottomRightCorner)
			{
				m_mouseBottomRightCornerState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Bottom Left Corner
		bool mouseIsOverBottomLeftCorner = RectContainsPoint(BottomLeftCornerRect(), e.GetX(), e.GetY());
		if (m_mouseBottomLeftCornerState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverBottomLeftCorner)
			{
				m_mouseBottomLeftCornerState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_NESW);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseBottomLeftCornerState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverBottomLeftCorner)
			{
				m_mouseBottomLeftCornerState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Right Edge
		bool mouseIsOverRightEdge = RectContainsPoint(RightEdgeRect(), e.GetX(), e.GetY());
		if (m_mouseRightEdgeState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverRightEdge)
			{
				m_mouseRightEdgeState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_EW);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseRightEdgeState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverRightEdge)
			{
				m_mouseRightEdgeState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Left Edge
		bool mouseIsOverLeftEdge = RectContainsPoint(LeftEdgeRect(), e.GetX(), e.GetY());
		if (m_mouseLeftEdgeState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverLeftEdge)
			{
				m_mouseLeftEdgeState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_EW);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseLeftEdgeState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverLeftEdge)
			{
				m_mouseLeftEdgeState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Top Edge
		bool mouseIsOverTopEdge = RectContainsPoint(TopEdgeRect(), e.GetX(), e.GetY());
		if (m_mouseTopEdgeState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverTopEdge)
			{
				m_mouseTopEdgeState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_NS);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseTopEdgeState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverTopEdge)
			{
				m_mouseTopEdgeState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Bottom Edge
		bool mouseIsOverBottomEdge = RectContainsPoint(BottomEdgeRect(), e.GetX(), e.GetY());
		if (m_mouseBottomEdgeState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverBottomEdge)
			{
				m_mouseBottomEdgeState = MouseOverDraggableAreaState::OVER;
				Window::SetCursor(Cursor::DOUBLE_ARROW_NS);
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseBottomEdgeState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverBottomEdge)
			{
				m_mouseBottomEdgeState = MouseOverDraggableAreaState::NOT_OVER;
				Window::SetCursor(Cursor::ARROW);
			}
			else
			{
				e.Handled(this);
				return;
			}
		}

		// Title Bar
		D2D1_RECT_F titleRect = TitleRect();
		bool mouseIsOverTitleBar =
			(m_paneCornerRadiusX > 0.0f && m_paneCornerRadiusY > 0.0f) ?
			RectContainsPoint(D2D1::RoundedRect(titleRect, m_paneCornerRadiusX, m_paneCornerRadiusY), e.GetX(), e.GetY()) :
			RectContainsPoint(titleRect, e.GetX(), e.GetY());

		if (m_mouseTitleBarState == MouseOverDraggableAreaState::NOT_OVER)
		{
			if (mouseIsOverTitleBar)
			{
				m_mouseTitleBarState = MouseOverDraggableAreaState::OVER;
				e.Handled(this);
				return;
			}
		}
		else if (m_mouseTitleBarState == MouseOverDraggableAreaState::OVER)
		{
			if (!mouseIsOverTitleBar)
			{
				m_mouseTitleBarState = MouseOverDraggableAreaState::NOT_OVER;
			}
		}
	}

	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseMove(e);

	// Finally, if the mouse is over the Pane but the event has not been handled, just let the Pane handle it
	if (!e.Handled() && RectContainsPoint(m_allowedRegion, e.GetX(), e.GetY()))
	{
		e.Handled(this);
	}
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

	// Handle mouse over draggable areas first before passing the event to the title layout
	// 
	// Top Right Corner
	if (m_mouseTopRightCornerState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseTopRightCornerState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseX = e.GetX(); 
		m_lastMouseY = e.GetY();
		e.Handled(this);
		return;
	}

	// Top Left Corner
	if (m_mouseTopLeftCornerState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseTopLeftCornerState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseX = e.GetX();
		m_lastMouseY = e.GetY();
		e.Handled(this);
		return;
	}

	// Bottom Right Corner
	if (m_mouseBottomRightCornerState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseBottomRightCornerState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseX = e.GetX();
		m_lastMouseY = e.GetY();
		e.Handled(this);
		return;
	}

	// Bottom Left Corner
	if (m_mouseBottomLeftCornerState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseBottomLeftCornerState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseX = e.GetX();
		m_lastMouseY = e.GetY();
		e.Handled(this);
		return;
	}

	// Right Edge
	if (m_mouseRightEdgeState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseRightEdgeState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseX = e.GetX();
		e.Handled(this);
		return;
	}

	// Left Edge
	if (m_mouseLeftEdgeState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseLeftEdgeState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseX = e.GetX();
		e.Handled(this);
		return;
	}

	// Top Edge
	if (m_mouseTopEdgeState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseTopEdgeState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseY = e.GetY();
		e.Handled(this);
		return;
	}

	// Bottom Edge
	if (m_mouseBottomEdgeState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseBottomEdgeState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseY = e.GetY();
		e.Handled(this);
		return;
	}

	// Title Bar
	if (m_mouseTitleBarState == MouseOverDraggableAreaState::OVER)
	{
		m_mouseTitleBarState = MouseOverDraggableAreaState::DRAGGING;
		m_lastMouseX = e.GetX();
		m_lastMouseY = e.GetY();
		e.Handled(this);
		return;
	}


	if (!e.Handled() && m_titleLayout != nullptr)
		m_titleLayout->OnMouseButtonPressed(e);


	if (!e.Handled() && !m_minimized)
		m_contentLayout->OnMouseButtonPressed(e);
}
void Pane::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");

	// Top Right Corner
	if (m_mouseTopRightCornerState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseTopRightCornerState = RectContainsPoint(TopRightCornerRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Top Left Corner
	if (m_mouseTopLeftCornerState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseTopLeftCornerState = RectContainsPoint(TopLeftCornerRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Bottom Right Corner
	if (m_mouseBottomRightCornerState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseBottomRightCornerState = RectContainsPoint(BottomRightCornerRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Bottom Left Corner
	if (m_mouseBottomLeftCornerState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseBottomLeftCornerState = RectContainsPoint(BottomLeftCornerRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Right Edge
	if (m_mouseRightEdgeState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseRightEdgeState = RectContainsPoint(RightEdgeRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Left Edge
	if (m_mouseLeftEdgeState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseLeftEdgeState = RectContainsPoint(LeftEdgeRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Top Edge
	if (m_mouseTopEdgeState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseTopEdgeState = RectContainsPoint(TopEdgeRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Bottom Edge
	if (m_mouseBottomEdgeState == MouseOverDraggableAreaState::DRAGGING)
	{
		m_mouseBottomEdgeState = RectContainsPoint(BottomEdgeRect(), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		Window::SetCursor(Cursor::ARROW);
		e.Handled(this);
		return;
	}

	// Title Bar
	if (m_mouseTitleBarState == MouseOverDraggableAreaState::DRAGGING)
	{
		D2D1_RECT_F titleRect = TitleRect();

		if (m_paneCornerRadiusX > 0.0f && m_paneCornerRadiusY > 0.0f)
		{
			m_mouseTitleBarState = RectContainsPoint(D2D1::RoundedRect(titleRect, m_paneCornerRadiusX, m_paneCornerRadiusY), e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		}
		else
		{
			m_mouseTitleBarState = RectContainsPoint(titleRect, e.GetX(), e.GetY()) ? MouseOverDraggableAreaState::OVER : MouseOverDraggableAreaState::NOT_OVER;
		}
		e.Handled(this);
		return;
	}

	if (!e.Handled() && m_titleLayout != nullptr)
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


bool Pane::RectContainsPoint(const D2D1_RECT_F& rect, float x, float y) noexcept
{
	return rect.left <= x && rect.right >= x && rect.top <= y && rect.bottom >= y;
}
bool Pane::RectContainsPoint(const D2D1_ROUNDED_RECT& rect, float x, float y)
{
	ComPtr<ID2D1RoundedRectangleGeometry> m_roundedRect = nullptr;
	GFX_THROW_INFO(
		m_deviceResources->D2DFactory()->CreateRoundedRectangleGeometry(rect, m_roundedRect.ReleaseAndGetAddressOf())
	)

	EG_CORE_ASSERT(m_roundedRect != nullptr, "rounded rect geometry should not be nullptr");

	BOOL b;
	m_roundedRect->FillContainsPoint(D2D1::Point2F(x, y), D2D1::Matrix3x2F::Identity(), &b);
	return static_cast<bool>(b);
}

}