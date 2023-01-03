#include "pch.h"
#include "ScrollableLayout.h"



namespace Evergreen
{
ScrollableLayout::ScrollableLayout(std::shared_ptr<DeviceResources> deviceResources,
									const D2D1_RECT_F& allowedRegion, 
									bool scrollVertical,
									bool scrollHorizontal,
									std::unique_ptr<ColorBrush> backgroundBrush,
									std::unique_ptr<ColorBrush> borderBrush,
									float borderWidth,
									const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, allowedRegion, margin),
	m_horizontalScrollOffset(0),
	m_verticalScrollOffset(0),
	m_canScrollHorizontal(scrollHorizontal),
	m_canScrollVertical(scrollVertical),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	//
	m_verticalScrollBarCornerXRadius(0.0f),
	m_verticalScrollBarCornerYRadius(0.0f),
	m_verticalScrollBarEnabled(scrollVertical),
	m_verticalScrollBarHiddenWhenNotDragging(true),
	m_verticalScrollBarWidth(8.0f),
	m_verticalScrollBarRegionWidth(12.0f),
	m_verticalScrollBarState(MouseOverBarState::NOT_OVER),

	m_horizontalScrollBarCornerXRadius(0.0f),
	m_horizontalScrollBarCornerYRadius(0.0f),
	m_horizontalScrollBarEnabled(scrollHorizontal),
	m_horizontalScrollBarHiddenWhenNotDragging(true),
	m_horizontalScrollBarHeight(10.0f),
	m_horizontalScrollBarRegionHeight(10.0f),
	m_horizontalScrollBarState(MouseOverBarState::NOT_OVER)
{
	// Brushes
	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	m_verticalScrollBarBrush			= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));
	m_verticalScrollBarBrushHovered		= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::DarkGray));
	m_verticalScrollBarBrushDragging	= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::LightGray));
	m_verticalScrollBarRegionBrush		= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::DimGray, 0.5f));

	m_horizontalScrollBarBrush			= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));
	m_horizontalScrollBarBrushHovered	= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::DarkGray));
	m_horizontalScrollBarBrushDragging	= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::LightGray));
	m_horizontalScrollBarRegionBrush	= std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::DimGray, 0.5f));

	// In the scrolling direction, set the height/width to 0. It will be adjusted when rows/columns are added
	float width  = m_canScrollHorizontal ? 0.0f : m_allowedRegion.right  - m_allowedRegion.left;
	float height = m_canScrollVertical   ? 0.0f : m_allowedRegion.bottom - m_allowedRegion.top;
	m_layout = std::make_unique<Layout>(deviceResources, m_allowedRegion.top, m_allowedRegion.left, width, height, nullptr, "ScrollableLayoutLayout");

	// Calling OnAllowedRegionChanged here will update the DrawRegion for each brush and subsequently call
	// ScrollableLayoutChanged() which will update all rect data
	OnAllowedRegionChanged();
}

void ScrollableLayout::Update() noexcept
{

}
void ScrollableLayout::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");

	ID2D1DeviceContext6* context = m_deviceResources->D2DDeviceContext();
	context->PushAxisAlignedClip(m_backgroundRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	// Draw the background
	context->FillRectangle(m_backgroundRect, m_backgroundBrush->Get());

	// Have the layout draw the background and contents of the brush
	m_layout->Render();

	context->PopAxisAlignedClip();

	// Vertical scroll bar
	if (m_verticalScrollBarEnabled)
	{
		context->FillRectangle(m_verticalScrollBarRegion, m_verticalScrollBarRegionBrush->Get());

		ColorBrush* brush = nullptr;
		switch(m_verticalScrollBarState)
		{
		case MouseOverBarState::NOT_OVER:	brush = m_verticalScrollBarBrush.get(); break;
		case MouseOverBarState::OVER:		brush = m_verticalScrollBarBrushHovered.get(); break;
		case MouseOverBarState::DRAGGING:	brush = m_verticalScrollBarBrushDragging.get(); break;
		}
		EG_CORE_ASSERT(brush != nullptr, "Something went wrong. brush should never be nullptr");

		if (m_verticalScrollBarCornerXRadius == 0.0f && m_verticalScrollBarCornerYRadius == 0.0f)
			context->FillRectangle(m_verticalScrollBar, brush->Get());
		else
			context->FillRoundedRectangle(D2D1::RoundedRect(m_verticalScrollBar, m_verticalScrollBarCornerXRadius, m_verticalScrollBarCornerYRadius), brush->Get());
	}

	// Draw the border last so it appears on top
	if (m_borderWidth > 0.0f)
		context->DrawRectangle(m_backgroundRect, m_borderBrush->Get(), m_borderWidth);
}

Row* ScrollableLayout::AddRow(RowColumnDefinition definition)
{
	if (m_canScrollVertical)
	{
		EG_CORE_ASSERT(definition.Type == RowColumnType::FIXED, "Row must be fixed if adding to layout that is vertically scrollable");
	
		// If vertical scrolling, then increase the height of the layout then add the new row
		m_layout->Resize(m_layout->Width(), m_layout->Height() + definition.Value);
	}

	return m_layout->AddRow(definition);
}
Column* ScrollableLayout::AddColumn(RowColumnDefinition definition)
{
	if (m_canScrollHorizontal)
	{
		EG_CORE_ASSERT(definition.Type == RowColumnType::FIXED, "Column must be fixed if adding to layout that is horizontally scrollable");

		// If vertical scrolling, then increase the height of the layout then add the new row
		m_layout->Resize(m_layout->Width() + definition.Value, m_layout->Height());
	}

	return m_layout->AddColumn(definition);
}
Layout* ScrollableLayout::AddSubLayout(RowColumnPosition position, const std::string& name)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	return m_layout->AddSubLayout(position, name);
}

void ScrollableLayout::ScrollableLayoutChanged()
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// Update the button rect to fill the allowed region minus the margin
	m_backgroundRect.left = m_allowedRegion.left + m_margin.Left;
	m_backgroundRect.top = m_allowedRegion.top + m_margin.Top;
	m_backgroundRect.right = m_allowedRegion.right - m_margin.Right;
	m_backgroundRect.bottom = m_allowedRegion.bottom - m_margin.Bottom;

	// In the scrolling direction, don't change the layout height/width
	float width  = m_canScrollHorizontal ? m_layout->Width()  : m_backgroundRect.right  - m_backgroundRect.left;
	float height = m_canScrollVertical   ? m_layout->Height() : m_backgroundRect.bottom - m_backgroundRect.top;

	m_layout->Resize(
		m_backgroundRect.top - m_verticalScrollOffset, 
		m_backgroundRect.left + m_horizontalScrollOffset, 
		width, 
		height
	);

	// scroll bar data -------------------------------------------------
	if (m_verticalScrollBarEnabled)
	{
		m_verticalScrollBarRegion = D2D1::RectF(m_backgroundRect.right - m_verticalScrollBarRegionWidth, m_backgroundRect.top, m_backgroundRect.right, m_backgroundRect.bottom);

		float barHeight = 20.0f;

		float barCenter		= m_verticalScrollBarRegion.left + ((m_verticalScrollBarRegion.right - m_verticalScrollBarRegion.left) / 2.0f);
		float halfBarWidth	= m_verticalScrollBarWidth / 2.0f;
		
		float barTop = m_verticalScrollBarRegion.top; // Default barTop value will be the top of the scroll region
		if (m_layout->Height() > m_backgroundRect.bottom - m_backgroundRect.top) // If the layout height exceeds the backgroundrect height, then scrolling is possible and we need to compute the top of the scroll rect
			barTop = 
					 (m_verticalScrollOffset / (m_layout->Height() - (m_backgroundRect.bottom - m_backgroundRect.top))) // This first part computes the percent the scroll offset is out of the total maximum scroll offsset
					 * (m_verticalScrollBarRegion.bottom - m_verticalScrollBarRegion.top - barHeight) // We then multiply that percent by the total space the scroll bar is allowed to move (Note: we subtract barHeight because we are looking for the offset of the top of the scroll bar)
					 + m_verticalScrollBarRegion.top; // Add the scroll region top to the computed offset
		
		m_verticalScrollBar = D2D1::RectF(
			barCenter - halfBarWidth,
			barTop,
			barCenter + halfBarWidth,
			barTop + barHeight
		);
	}
}
void ScrollableLayout::OnMarginChanged()
{
	ScrollableLayoutChanged();
}
void ScrollableLayout::OnAllowedRegionChanged()
{
	EG_CORE_ASSERT(m_borderBrush != nullptr, "No border brush");
	EG_CORE_ASSERT(m_backgroundBrush != nullptr, "No background brush");

	// If using a non-SolidColorBrush, we need to update the draw region for the brushes
	m_backgroundBrush->SetDrawRegion(m_allowedRegion);
	m_borderBrush->SetDrawRegion(m_allowedRegion);

	ScrollableLayoutChanged();
}



void ScrollableLayout::OnChar(CharEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnChar(e);
}
void ScrollableLayout::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnKeyPressed(e);
}
void ScrollableLayout::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnKeyReleased(e);
}
void ScrollableLayout::OnMouseMove(MouseMoveEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnMouseMove(e);
}
void ScrollableLayout::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// First determine if a child control wants to handle the event
	m_layout->OnMouseScrolledVertical(e);
	if (e.Handled())
		return;

	if (m_canScrollVertical)
	{
		float visibleHeight = m_backgroundRect.bottom - m_backgroundRect.top;

		// When using a mouse pad, the scroll deltas are usually in the range [1-10]. When using a mouse wheel, the deltas
		// are usually +/-120. So if we get a large value, just divide it by 10
		m_verticalScrollOffset -= (std::abs(e.GetScrollDelta()) < 100 ? e.GetScrollDelta() : e.GetScrollDelta() / 10);
		m_verticalScrollOffset = std::max(0, m_verticalScrollOffset);
		m_verticalScrollOffset = std::min(m_verticalScrollOffset, static_cast<int>(m_layout->Height() - visibleHeight));
		ScrollableLayoutChanged();
		e.Handled(this);
	}
}
void ScrollableLayout::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");

	// First determine if a child control wants to handle the event
	m_layout->OnMouseScrolledHorizontal(e);
	if (e.Handled())
		return;

	if (m_canScrollHorizontal)
	{
		float visibleWidth = m_backgroundRect.right - m_backgroundRect.left;

		// When using a mouse pad, the scroll deltas are usually in the range [1-10]. When using a mouse wheel, the deltas
		// are usually +/-120. So if we get a large value, just divide it by 10
		m_horizontalScrollOffset -= (std::abs(e.GetScrollDelta()) < 100 ? e.GetScrollDelta() : e.GetScrollDelta() / 10);
		m_horizontalScrollOffset = std::min(0, m_horizontalScrollOffset);
		m_horizontalScrollOffset = std::max(m_horizontalScrollOffset, -1 * static_cast<int>(m_layout->Width() - visibleWidth));
		ScrollableLayoutChanged();
		e.Handled(this);
	}
}
void ScrollableLayout::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnMouseButtonPressed(e);
}
void ScrollableLayout::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnMouseButtonReleased(e);
}
void ScrollableLayout::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnMouseButtonDoubleClick(e);
}
}