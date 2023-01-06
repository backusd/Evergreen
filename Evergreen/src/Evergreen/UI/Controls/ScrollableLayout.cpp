#include "pch.h"
#include "ScrollableLayout.h"

using Microsoft::WRL::ComPtr;

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
	m_horizontalScrollOffset(0.0f),
	m_verticalScrollOffset(0.0f),
	m_canScrollHorizontal(scrollHorizontal),
	m_canScrollVertical(scrollVertical),
	m_backgroundBrush(std::move(backgroundBrush)),
	m_borderBrush(std::move(borderBrush)),
	m_borderWidth(borderWidth),
	//
	m_verticalScrollBarCornerXRadius(0.0f),
	m_verticalScrollBarCornerYRadius(0.0f),
	m_verticalScrollBarEnabled(scrollVertical),
	m_verticalScrollBarHiddenWhenNotOver(true),
	m_verticalScrollBarWidth(8.0f),
	m_verticalScrollBarRegionWidth(12.0f),
	m_verticalScrollBarState(MouseOverBarState::NOT_OVER),
	m_mouseIsOverVerticalScrollBarRegion(false),

	m_horizontalScrollBarCornerXRadius(0.0f),
	m_horizontalScrollBarCornerYRadius(0.0f),
	m_horizontalScrollBarEnabled(scrollHorizontal),
	m_horizontalScrollBarHiddenWhenNotOver(true),
	m_horizontalScrollBarHeight(10.0f),
	m_horizontalScrollBarRegionHeight(10.0f),
	m_horizontalScrollBarState(MouseOverBarState::NOT_OVER),
	m_mouseIsOverHorizontalScrollBarRegion(false),

	m_dragStartPoint(D2D1::Point2F())
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
	if (m_verticalScrollBarEnabled && 
		(!m_verticalScrollBarHiddenWhenNotOver || m_mouseIsOverVerticalScrollBarRegion || m_verticalScrollBarState == MouseOverBarState::DRAGGING))
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

	// Horizontal scroll bar
	if (m_horizontalScrollBarEnabled &&
		(!m_horizontalScrollBarHiddenWhenNotOver || m_mouseIsOverHorizontalScrollBarRegion || m_horizontalScrollBarState == MouseOverBarState::DRAGGING))
	{
		context->FillRectangle(m_horizontalScrollBarRegion, m_horizontalScrollBarRegionBrush->Get());

		ColorBrush* brush = nullptr;
		switch (m_horizontalScrollBarState)
		{
		case MouseOverBarState::NOT_OVER:	brush = m_horizontalScrollBarBrush.get(); break;
		case MouseOverBarState::OVER:		brush = m_horizontalScrollBarBrushHovered.get(); break;
		case MouseOverBarState::DRAGGING:	brush = m_horizontalScrollBarBrushDragging.get(); break;
		}
		EG_CORE_ASSERT(brush != nullptr, "Something went wrong. brush should never be nullptr");

		if (m_horizontalScrollBarCornerXRadius == 0.0f && m_horizontalScrollBarCornerYRadius == 0.0f)
			context->FillRectangle(m_horizontalScrollBar, brush->Get());
		else
			context->FillRoundedRectangle(D2D1::RoundedRect(m_horizontalScrollBar, m_horizontalScrollBarCornerXRadius, m_horizontalScrollBarCornerYRadius), brush->Get());
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

void ScrollableLayout::VerticalScrollBarBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_verticalScrollBarBrush = std::move(brush); 
	m_verticalScrollBarBrush->SetDrawRegion(m_verticalScrollBar); 
}
void ScrollableLayout::VerticalScrollBarHoveredBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_verticalScrollBarBrushHovered = std::move(brush); 
	m_verticalScrollBarBrushHovered->SetDrawRegion(m_verticalScrollBar); 
}
void ScrollableLayout::VerticalScrollBarDraggingBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_verticalScrollBarBrushDragging = std::move(brush); 
	m_verticalScrollBarBrushDragging->SetDrawRegion(m_verticalScrollBar); 
}
void ScrollableLayout::VerticalScrollBarRegionBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_verticalScrollBarRegionBrush = std::move(brush); 
	m_verticalScrollBarRegionBrush->SetDrawRegion(m_verticalScrollBarRegion); 
}
void ScrollableLayout::HorizontalScrollBarBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_horizontalScrollBarBrush = std::move(brush); 
	m_horizontalScrollBarBrush->SetDrawRegion(m_horizontalScrollBar);
}
void ScrollableLayout::HorizontalScrollBarHoveredBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_horizontalScrollBarBrushHovered = std::move(brush);
	m_horizontalScrollBarBrushHovered->SetDrawRegion(m_horizontalScrollBar);
}
void ScrollableLayout::HorizontalScrollBarDraggingBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_horizontalScrollBarBrushDragging = std::move(brush); 
	m_horizontalScrollBarBrushDragging->SetDrawRegion(m_horizontalScrollBar);
}
void ScrollableLayout::HorizontalScrollBarRegionBrush(std::unique_ptr<ColorBrush> brush) noexcept 
{ 
	m_horizontalScrollBarRegionBrush = std::move(brush); 
	m_horizontalScrollBarRegionBrush->SetDrawRegion(m_horizontalScrollBarRegion);
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

	VerticalScrollBarChanged();
	HorizontalScrollBarChanged();
}
void ScrollableLayout::VerticalScrollBarChanged() noexcept
{
	if (m_verticalScrollBarEnabled)
	{
		m_verticalScrollBarRegion = D2D1::RectF(m_backgroundRect.right - m_verticalScrollBarRegionWidth, m_backgroundRect.top, m_backgroundRect.right, m_backgroundRect.bottom);

		float barHeight = 20.0f;

		float barCenter = m_verticalScrollBarRegion.left + ((m_verticalScrollBarRegion.right - m_verticalScrollBarRegion.left) / 2.0f);
		float halfBarWidth = m_verticalScrollBarWidth / 2.0f;

		float barTop = m_verticalScrollBarRegion.top; // Default barTop value will be the top of the scroll region
		if (m_layout->Height() > m_backgroundRect.bottom - m_backgroundRect.top) // If the layout height exceeds the backgroundrect height, then scrolling is possible and we need to compute the top of the scroll rect
		{
			barTop =
				(m_verticalScrollOffset / (m_layout->Height() - (m_backgroundRect.bottom - m_backgroundRect.top))) // This first part computes the percent the scroll offset is out of the total maximum scroll offset
				* (m_verticalScrollBarRegion.bottom - m_verticalScrollBarRegion.top - barHeight) // We then multiply that percent by the total space the scroll bar is allowed to move (Note: we subtract barHeight because we are looking for the offset of the top of the scroll bar)
				+ m_verticalScrollBarRegion.top; // Add the scroll region top to the computed offset
		}

		m_verticalScrollBar = D2D1::RectF(
			barCenter - halfBarWidth,
			barTop,
			barCenter + halfBarWidth,
			barTop + barHeight
		);
	}

	m_verticalScrollBarBrush->SetDrawRegion(m_verticalScrollBar);
	m_verticalScrollBarBrushHovered->SetDrawRegion(m_verticalScrollBar);
	m_verticalScrollBarBrushDragging->SetDrawRegion(m_verticalScrollBar);
	m_verticalScrollBarRegionBrush->SetDrawRegion(m_verticalScrollBarRegion);
}
void ScrollableLayout::HorizontalScrollBarChanged() noexcept
{
	if (m_horizontalScrollBarEnabled)
	{
		m_horizontalScrollBarRegion = D2D1::RectF(m_backgroundRect.left, m_backgroundRect.bottom - m_horizontalScrollBarRegionHeight, m_backgroundRect.right, m_backgroundRect.bottom);

		float barWidth = 20.0f;

		float barCenter = m_horizontalScrollBarRegion.top + ((m_horizontalScrollBarRegion.bottom - m_horizontalScrollBarRegion.top) / 2.0f);
		float halfBarHeight = m_horizontalScrollBarHeight / 2.0f;

		float barLeft = m_horizontalScrollBarRegion.left; // Default barLeft value will be the left of the scroll region
		if (m_layout->Width() > m_backgroundRect.right - m_backgroundRect.left) // If the layout height exceeds the backgroundrect width, then scrolling is possible and we need to compute the left of the scroll rect
		{
			barLeft =
				(-m_horizontalScrollOffset / (m_layout->Width() - (m_backgroundRect.right - m_backgroundRect.left))) // This first part computes the percent the scroll offset is out of the total maximum scroll offset
				* (m_horizontalScrollBarRegion.right - m_horizontalScrollBarRegion.left - barWidth) // We then multiply that percent by the total space the scroll bar is allowed to move (Note: we subtract barWidth because we are looking for the offset of the left of the scroll bar)
				+ m_horizontalScrollBarRegion.left; // Add the scroll region left to the computed offset
		}

		m_horizontalScrollBar = D2D1::RectF(
			barLeft,
			barCenter - halfBarHeight,
			barLeft + barWidth,
			barCenter + halfBarHeight
		);
	}

	m_horizontalScrollBarBrush->SetDrawRegion(m_horizontalScrollBar);
	m_horizontalScrollBarBrushHovered->SetDrawRegion(m_horizontalScrollBar);
	m_horizontalScrollBarBrushDragging->SetDrawRegion(m_horizontalScrollBar);
	m_horizontalScrollBarRegionBrush->SetDrawRegion(m_horizontalScrollBarRegion);

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

bool ScrollableLayout::RectContainsPoint(const D2D1_RECT_F& rect, float x, float y) noexcept
{
	return rect.left <= x && rect.right >= x && rect.top <= y && rect.bottom >= y;
}
bool ScrollableLayout::RectContainsPoint(const D2D1_ROUNDED_RECT& rect, float x, float y)
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

void ScrollableLayout::IncrementVerticalScrollOffset(float delta)
{
	// This function is supposed to be called to make a change to m_verticalScrollOffset
	// It will ensure that the offset is within the correct bounds and update the vertical scroll bar rect

	float visibleHeight = m_backgroundRect.bottom - m_backgroundRect.top;

	m_verticalScrollOffset += delta;
	m_verticalScrollOffset = std::max(0.0f, m_verticalScrollOffset);
	m_verticalScrollOffset = std::min(m_verticalScrollOffset, m_layout->Height() - visibleHeight);

	ScrollableLayoutChanged();
}
void ScrollableLayout::IncrementHorizontalScrollOffset(float delta)
{
	float visibleWidth = m_backgroundRect.right - m_backgroundRect.left;

	m_horizontalScrollOffset += delta;
	m_horizontalScrollOffset = std::min(0.0f, m_horizontalScrollOffset);
	m_horizontalScrollOffset = std::max(m_horizontalScrollOffset, visibleWidth - m_layout->Width());
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
	// If mouse is not interacting with either scroll bar and also not over the background rect, just return
	// NOTE: You can NOT just test to see if the Mouse-over-bar states are NOT_DRAGGING because there is the scenario
	//       where the mouse is OVER a bar and then the next MouseMove event the mouse is off the bar and ALSO out of the entire
	//       control. In which case, we need to set the state back to NOT_OVER, which is what some of the code does below
	if (m_verticalScrollBarState == MouseOverBarState::NOT_OVER &&
		m_horizontalScrollBarState == MouseOverBarState::NOT_OVER &&
		!RectContainsPoint(m_backgroundRect, e.GetX(), e.GetY()))
	{
		m_mouseIsOverVerticalScrollBarRegion = false;
		m_mouseIsOverHorizontalScrollBarRegion = false;
		return;
	}

	// Vertical --------------------------------------------------------------------------------------------
	if (m_canScrollVertical)
	{
		m_mouseIsOverVerticalScrollBarRegion = RectContainsPoint(m_verticalScrollBarRegion, e.GetX(), e.GetY());

		// We are going to let the scrollbars take precedence over the layout for handling the event
		if (m_verticalScrollBarState == MouseOverBarState::DRAGGING)
		{
			// NOTE: We are about to compute the vertical delta that the mouse moved and want to use that value to adjust m_verticalScrollOffset
			// However we want the mouse to track with the scroll bar rect as it is being dragged. In order to do this, we must convert
			// the vertical pixel delta to an appropriate offset delta. The way to think about this that we need compute "offset per pixel", which
			// is just the maximum amount of allowed offset divided by the maximum change in pixels for the scroll bar rect itself
			float offsetPerPixel = (m_layout->Height() - (m_verticalScrollBarRegion.bottom - m_verticalScrollBarRegion.top)) / (m_verticalScrollBarRegion.bottom - m_verticalScrollBarRegion.top - (m_verticalScrollBar.bottom - m_verticalScrollBar.top));
			IncrementVerticalScrollOffset((e.GetY() - m_dragStartPoint.y) * offsetPerPixel);
			m_dragStartPoint.y = e.GetY();

			e.Handled(this);
			return;
		}

		if (m_verticalScrollBarState == MouseOverBarState::OVER)
		{
			// First do a check to see if the mouse is over scrollbar rect assuming no rounded corners (this check is fast)
			if (RectContainsPoint(m_verticalScrollBar, e.GetX(), e.GetY()))
			{
				// Next either confirm that there are no rounded corners or if there are, verify the mouse is over the rounded rect (slightly slower check)
				if ((m_verticalScrollBarCornerXRadius == 0.0f && m_verticalScrollBarCornerYRadius == 0.0f) ||
					RectContainsPoint(D2D1::RoundedRect(m_verticalScrollBar, m_verticalScrollBarCornerXRadius, m_verticalScrollBarCornerYRadius), e.GetX(), e.GetY()))
				{
					e.Handled(this);
					return;
				}
			}

			m_verticalScrollBarState = MouseOverBarState::NOT_OVER;
			return;
		}

		if (m_verticalScrollBarState == MouseOverBarState::NOT_OVER)
		{
			// First do a check to see if the mouse is over scrollbar rect assuming no rounded corners (this check is fast)
			if (RectContainsPoint(m_verticalScrollBar, e.GetX(), e.GetY()))
			{
				// Next either confirm that there are no rounded corners or if there are, verify the mouse is over the rounded rect (slightly slower check)
				if ((m_verticalScrollBarCornerXRadius == 0.0f && m_verticalScrollBarCornerYRadius == 0.0f) ||
					RectContainsPoint(D2D1::RoundedRect(m_verticalScrollBar, m_verticalScrollBarCornerXRadius, m_verticalScrollBarCornerYRadius), e.GetX(), e.GetY()))
				{
					m_verticalScrollBarState = MouseOverBarState::OVER;
					e.Handled(this);
					return;
				}
			}
		}
	}

	// Horizontal --------------------------------------------------------------------------------------------
	if (m_canScrollHorizontal)
	{
		m_mouseIsOverHorizontalScrollBarRegion = RectContainsPoint(m_horizontalScrollBarRegion, e.GetX(), e.GetY());

		// We are going to let the scrollbars take precedence over the layout for handling the event
		if (m_horizontalScrollBarState == MouseOverBarState::DRAGGING)
		{
			// See the NOTE above in the vertical section 
			float offsetPerPixel = (m_layout->Width() - (m_horizontalScrollBarRegion.right - m_horizontalScrollBarRegion.left)) / (m_horizontalScrollBarRegion.right - m_horizontalScrollBarRegion.left - (m_horizontalScrollBar.right - m_horizontalScrollBar.left));
			IncrementHorizontalScrollOffset((m_dragStartPoint.x - e.GetX()) * offsetPerPixel);
			m_dragStartPoint.x = e.GetX();

			e.Handled(this);
			return;
		}

		if (m_horizontalScrollBarState == MouseOverBarState::OVER)
		{
			// First do a check to see if the mouse is over scrollbar rect assuming no rounded corners (this check is fast)
			if (RectContainsPoint(m_horizontalScrollBar, e.GetX(), e.GetY()))
			{
				// Next either confirm that there are no rounded corners or if there are, verify the mouse is over the rounded rect (slightly slower check)
				if ((m_horizontalScrollBarCornerXRadius == 0.0f && m_horizontalScrollBarCornerYRadius == 0.0f) ||
					RectContainsPoint(D2D1::RoundedRect(m_horizontalScrollBar, m_horizontalScrollBarCornerXRadius, m_horizontalScrollBarCornerYRadius), e.GetX(), e.GetY()))
				{
					e.Handled(this);
					return;
				}
			}

			m_horizontalScrollBarState = MouseOverBarState::NOT_OVER;
			return;
		}

		if (m_horizontalScrollBarState == MouseOverBarState::NOT_OVER)
		{
			// First do a check to see if the mouse is over scrollbar rect assuming no rounded corners (this check is fast)
			if (RectContainsPoint(m_horizontalScrollBar, e.GetX(), e.GetY()))
			{
				// Next either confirm that there are no rounded corners or if there are, verify the mouse is over the rounded rect (slightly slower check)
				if ((m_horizontalScrollBarCornerXRadius == 0.0f && m_horizontalScrollBarCornerYRadius == 0.0f) ||
					RectContainsPoint(D2D1::RoundedRect(m_horizontalScrollBar, m_horizontalScrollBarCornerXRadius, m_horizontalScrollBarCornerYRadius), e.GetX(), e.GetY()))
				{
					m_horizontalScrollBarState = MouseOverBarState::OVER;
					e.Handled(this);
					return;
				}
			}
		}
	}








	// Finally, if the scrollbars have not handled the event, forward it to the layout
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
		// When using a mouse pad, the scroll deltas are usually in the range [1-10]. When using a mouse wheel, the deltas
		// are usually +/-120. So if we get a large value, just divide it by 10
		IncrementVerticalScrollOffset(-1.0f * static_cast<float>(std::abs(e.GetScrollDelta()) < 100 ? e.GetScrollDelta() : e.GetScrollDelta() / 10));
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
		// When using a mouse pad, the scroll deltas are usually in the range [1-10]. When using a mouse wheel, the deltas
		// are usually +/-120. So if we get a large value, just divide it by 10
		IncrementHorizontalScrollOffset(-1.0f * static_cast<float>(std::abs(e.GetScrollDelta()) < 100 ? e.GetScrollDelta() : e.GetScrollDelta() / 10));
		e.Handled(this);
	}
}
void ScrollableLayout::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	// ScrollLayout should only handle/pass this event if the press event is over the control
	if (!RectContainsPoint(m_backgroundRect, e.GetX(), e.GetY()))
	{
		return;
	}

	// NOTE: No need to check m_canScrollVertical/m_canScrollHorizontal because if they are false they can never get to the
	//       MouseOverBarState::OVER state
	if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		if (m_verticalScrollBarState == MouseOverBarState::OVER)
		{
			m_verticalScrollBarState = MouseOverBarState::DRAGGING;
			m_dragStartPoint = D2D1::Point2F(e.GetX(), e.GetY());
			e.Handled(this);
			return;
		}

		if (m_horizontalScrollBarState == MouseOverBarState::OVER)
		{
			m_horizontalScrollBarState = MouseOverBarState::DRAGGING;
			m_dragStartPoint = D2D1::Point2F(e.GetX(), e.GetY());
			e.Handled(this);
			return;
		}
	}

	// Not handled so pass to layout
	m_layout->OnMouseButtonPressed(e);
}
void ScrollableLayout::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		if (m_verticalScrollBarState == MouseOverBarState::DRAGGING)
		{
			// First do a check to see if the mouse is over scrollbar rect assuming no rounded corners (this check is fast)
			if (RectContainsPoint(m_verticalScrollBar, e.GetX(), e.GetY()))
			{
				// Next either confirm that there are no rounded corners or if there are, verify the mouse is over the rounded rect (slightly slower check)
				if ((m_verticalScrollBarCornerXRadius == 0.0f && m_verticalScrollBarCornerYRadius == 0.0f) ||
					RectContainsPoint(D2D1::RoundedRect(m_verticalScrollBar, m_verticalScrollBarCornerXRadius, m_verticalScrollBarCornerYRadius), e.GetX(), e.GetY()))
				{
					m_verticalScrollBarState = MouseOverBarState::OVER;
					e.Handled(this);
					return;
				}
			}

			m_verticalScrollBarState = MouseOverBarState::NOT_OVER;
			e.Handled(this);
			return;
		}

		if (m_horizontalScrollBarState == MouseOverBarState::DRAGGING)
		{
			// First do a check to see if the mouse is over scrollbar rect assuming no rounded corners (this check is fast)
			if (RectContainsPoint(m_horizontalScrollBar, e.GetX(), e.GetY()))
			{
				// Next either confirm that there are no rounded corners or if there are, verify the mouse is over the rounded rect (slightly slower check)
				if ((m_horizontalScrollBarCornerXRadius == 0.0f && m_horizontalScrollBarCornerYRadius == 0.0f) ||
					RectContainsPoint(D2D1::RoundedRect(m_horizontalScrollBar, m_horizontalScrollBarCornerXRadius, m_horizontalScrollBarCornerYRadius), e.GetX(), e.GetY()))
				{
					m_horizontalScrollBarState = MouseOverBarState::OVER;
					e.Handled(this);
					return;
				}
			}

			m_horizontalScrollBarState = MouseOverBarState::NOT_OVER;
			e.Handled(this);
			return;
		}
	}

	// Not handled so pass to layout 
	m_layout->OnMouseButtonReleased(e);
}
void ScrollableLayout::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	// ScrollableLayout doesn't need to handle this, but should forward it to its layout 
	m_layout->OnMouseButtonDoubleClick(e);
}
}