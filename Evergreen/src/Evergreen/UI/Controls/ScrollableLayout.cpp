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
	m_borderWidth(borderWidth)
{
	if (m_backgroundBrush == nullptr)
		m_backgroundBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray));

	if (m_borderBrush == nullptr)
		m_borderBrush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	// In the scrolling direction, set the height/width to 0. It will be adjusted when rows/columns are added
	float width  = m_canScrollHorizontal ? 0.0f : m_allowedRegion.right  - m_allowedRegion.left;
	float height = m_canScrollVertical   ? 0.0f : m_allowedRegion.bottom - m_allowedRegion.top;

	m_layout = std::make_unique<Layout>(deviceResources, m_allowedRegion.top, m_allowedRegion.left, width, height, nullptr, "ScrollableLayoutLayout");

	// This will update the background rect
	ScrollableLayoutChanged();
}

void ScrollableLayout::Update() const noexcept
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

		m_verticalScrollOffset -= e.GetScrollDelta();
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

		m_horizontalScrollOffset -= e.GetScrollDelta();
		m_horizontalScrollOffset = std::min(0, m_horizontalScrollOffset);
		m_horizontalScrollOffset = std::max(m_horizontalScrollOffset, -1 * static_cast<int>(m_layout->Width() - visibleWidth));
		ScrollableLayoutChanged();
		e.Handled(this);
	}
}

}