#include "pch.h"
#include "Layout.h"

namespace Evergreen
{
// Row ------------------------------------------------------------------------------
Row::Row(float top, float left, float width, float height, float maxHeight, float minHeight, bool topAdjustable, bool bottomAdjustable) noexcept :
	m_top(top), m_left(left), m_width(width), m_height(height), 
	m_maxHeight(maxHeight), m_minHeight(minHeight), 
	m_topAdjustable(topAdjustable), m_bottomAdjustable(bottomAdjustable)
{
}

// Column ------------------------------------------------------------------------------
Column::Column(float top, float left, float width, float height, float maxWidth, float minWidth, bool leftAdjustable, bool rightAdjustable) noexcept :
	m_top(top), m_left(left), m_width(width), m_height(height),
	m_maxWidth(maxWidth), m_minWidth(minWidth),
	m_leftAdjustable(leftAdjustable), m_rightAdjustable(rightAdjustable)
{
}



// Layout ------------------------------------------------------------------------------
Layout::Layout(float top, float left, float width, float height) noexcept :
	m_top(top), m_left(left), m_width(width), m_height(height)
{
	// Leave rows and columns empty for now
}

void Layout::AddRow(RowColumnDefinition definition) noexcept
{
	m_rowDefinitions.push_back(definition);
	UpdateLayout();
}

void Layout::AddColumn(RowColumnDefinition definition) noexcept
{
	m_columnDefinitions.push_back(definition);
	UpdateLayout();
}

void Layout::UpdateLayout() noexcept
{

}


}