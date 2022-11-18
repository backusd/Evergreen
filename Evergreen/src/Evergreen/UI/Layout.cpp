#include "pch.h"
#include "Layout.h"
#include "Evergreen/Utils/SetCursor.h"

namespace Evergreen
{
// Row ------------------------------------------------------------------------------
Row::Row(float top, float left, float width, float height, RowColumnDefinition maxHeightDef, RowColumnDefinition minHeightDef, bool topAdjustable, bool bottomAdjustable) noexcept :
	m_top(top), m_left(left), m_width(width), m_height(height), 
	m_starHeight(height), m_parentLayoutHeight(height),
	m_topAdjustable(topAdjustable), m_bottomAdjustable(bottomAdjustable),
	m_maxHeightDefinition(maxHeightDef),
	m_minHeightDefinition(minHeightDef)
{
}
Row::Row(const Row& rhs) noexcept :
	m_top(rhs.m_top), m_left(rhs.m_left), m_width(rhs.m_width), m_height(rhs.m_height), 
	m_starHeight(rhs.m_starHeight), m_parentLayoutHeight(rhs.m_parentLayoutHeight),
	m_maxHeightDefinition(rhs.m_maxHeightDefinition), m_minHeightDefinition(rhs.m_minHeightDefinition),
	m_topAdjustable(rhs.m_topAdjustable), m_bottomAdjustable(rhs.m_bottomAdjustable)
{
}
void Row::operator=(const Row& rhs) noexcept
{
	this->m_top = rhs.m_top;
	this->m_left = rhs.m_left;
	this->m_width = rhs.m_width;
	this->m_height = rhs.m_height;
	this->m_starHeight = rhs.m_starHeight;
	this->m_parentLayoutHeight = rhs.m_parentLayoutHeight;
	this->m_maxHeightDefinition = rhs.m_maxHeightDefinition;
	this->m_minHeightDefinition = rhs.m_minHeightDefinition;
	this->m_topAdjustable = rhs.m_topAdjustable;
	this->m_bottomAdjustable = rhs.m_bottomAdjustable;
}
float Row::MaxHeight() const noexcept
{
	switch (m_maxHeightDefinition.Type)
	{
	case RowColumnType::FIXED:	 return m_maxHeightDefinition.Value;
	case RowColumnType::PERCENT: return m_parentLayoutHeight * m_maxHeightDefinition.Value;
	case RowColumnType::STAR:	 return m_starHeight * m_maxHeightDefinition.Value;
	}

	return FLT_MAX;
}
float Row::MinHeight() const noexcept
{
	switch (m_minHeightDefinition.Type)
	{
	case RowColumnType::FIXED:	 return m_minHeightDefinition.Value;
	case RowColumnType::PERCENT: return m_parentLayoutHeight * m_minHeightDefinition.Value;
	case RowColumnType::STAR:	 return m_starHeight * m_minHeightDefinition.Value;
	}

	return 1.0f;
}

// Column ------------------------------------------------------------------------------
Column::Column(float top, float left, float width, float height, RowColumnDefinition maxWidthDef, RowColumnDefinition minWidthDef, bool leftAdjustable, bool rightAdjustable) noexcept :
	m_top(top), m_left(left), m_width(width), m_height(height), 
	m_starWidth(width), m_parentLayoutWidth(width),
	m_leftAdjustable(leftAdjustable), m_rightAdjustable(rightAdjustable),
	m_maxWidthDefinition(maxWidthDef),
	m_minWidthDefinition(minWidthDef)
{
}
Column::Column(const Column& rhs) noexcept :
	m_top(rhs.m_top), m_left(rhs.m_left), m_width(rhs.m_width), m_height(rhs.m_height),
	m_starWidth(rhs.m_starWidth), m_parentLayoutWidth(rhs.m_parentLayoutWidth),
	m_maxWidthDefinition(rhs.m_maxWidthDefinition), m_minWidthDefinition(rhs.m_minWidthDefinition),
	m_leftAdjustable(rhs.m_leftAdjustable), m_rightAdjustable(rhs.m_rightAdjustable)
{
}
void Column::operator=(const Column& rhs) noexcept
{
	this->m_top = rhs.m_top;
	this->m_left = rhs.m_left;
	this->m_width = rhs.m_width;
	this->m_height = rhs.m_height;
	this->m_starWidth = rhs.m_starWidth;
	this->m_parentLayoutWidth = rhs.m_parentLayoutWidth;
	this->m_maxWidthDefinition = rhs.m_maxWidthDefinition;
	this->m_minWidthDefinition = rhs.m_minWidthDefinition;
	this->m_leftAdjustable = rhs.m_leftAdjustable;
	this->m_rightAdjustable = rhs.m_rightAdjustable;
}
float Column::MaxWidth() const noexcept
{
	switch (m_maxWidthDefinition.Type)
	{
	case RowColumnType::FIXED:	 return m_maxWidthDefinition.Value;
	case RowColumnType::PERCENT: return m_parentLayoutWidth * m_maxWidthDefinition.Value;
	case RowColumnType::STAR:	 return m_starWidth * m_maxWidthDefinition.Value;
	}

	return FLT_MAX;
}
float Column::MinWidth() const noexcept
{
	switch (m_minWidthDefinition.Type)
	{
	case RowColumnType::FIXED:	 return m_minWidthDefinition.Value;
	case RowColumnType::PERCENT: return m_parentLayoutWidth * m_minWidthDefinition.Value;
	case RowColumnType::STAR:	 return m_starWidth * m_minWidthDefinition.Value;
	}

	return 1.0f;
}

// Layout ------------------------------------------------------------------------------
Layout::Layout(float top, float left, float width, float height, const std::string& name) noexcept :
	m_top(top), m_left(left), m_width(width), m_height(height), m_name(name),
	m_activelyAdjustingLayout(false)
{
	// Leave rows and columns empty for now
}

std::optional<Row*> Layout::AddRow(RowColumnDefinition definition) noexcept
{
	switch (definition.Type)
	{
	case RowColumnType::FIXED:		
		if (definition.Value < 0.0f)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddRow, with an invalid FIXED value ({}). Must be between greater than 0", __FILE__, __LINE__, m_name, definition.Value);
			EG_CORE_ERROR("    Unable to add row -> Ignoring it");
			return std::nullopt;
		}
		else if (definition.Value > m_height)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddRow, with a FIXED height ({}) greater than the layout's current height ({})", __FILE__, __LINE__, m_name, definition.Value, m_height);
			EG_CORE_ERROR("    Unable to add row -> Ignoring it");
			return std::nullopt;
		}
		break;

	case RowColumnType::PERCENT:
		if (definition.Value > 1.0f || definition.Value < 0.0f)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddRow, with an invalid PERCENT height ({}). Must be between [0, 1]", __FILE__, __LINE__, m_name, definition.Value);
			EG_CORE_ERROR("    Unable to add row -> Ignoring it");
			return std::nullopt;
		}
		break;

	case RowColumnType::STAR:
		if (definition.Value < 0.0f)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddRow, with an invalid STAR value ({}). Must be between greater than 0", __FILE__, __LINE__, m_name, definition.Value);
			EG_CORE_ERROR("    Unable to add row -> Ignoring it");
			return std::nullopt;
		}
		break;
	}

	m_rowDefinitions.push_back(definition);
	UpdateRows();

	return &m_rows.back();
}
std::optional<Column*> Layout::AddColumn(RowColumnDefinition definition) noexcept
{
	switch (definition.Type)
	{
	case RowColumnType::FIXED:
		if (definition.Value < 0.0f)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddColumn, with an invalid FIXED value ({}). Must be between greater than 0", __FILE__, __LINE__, m_name, definition.Value);
			EG_CORE_ERROR("    Unable to add column -> Ignoring it");
			return std::nullopt;
		}
		else if (definition.Value > m_height)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddColumn, with a FIXED width ({}) greater than the layout's current width ({})", __FILE__, __LINE__, m_name, definition.Value, m_width);
			EG_CORE_ERROR("    Unable to add column -> Ignoring it");
			return std::nullopt;
		}
		break;

	case RowColumnType::PERCENT:
		if (definition.Value > 1.0f || definition.Value < 0.0f)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddColumn, with an invalid PERCENT height ({}). Must be between [0, 1]", __FILE__, __LINE__, m_name, definition.Value);
			EG_CORE_ERROR("    Unable to add column -> Ignoring it");
			return std::nullopt;
		}
		break;

	case RowColumnType::STAR:
		if (definition.Value < 0.0f)
		{
			EG_CORE_ERROR("{}:{} - Layout (name: {}) called AddColumn, with an invalid STAR value ({}). Must be between greater than 0", __FILE__, __LINE__, m_name, definition.Value);
			EG_CORE_ERROR("    Unable to add column -> Ignoring it");
			return std::nullopt;
		}
		break;
	}

	m_columnDefinitions.push_back(definition);
	UpdateColumns();

	return &m_columns.back();
}

void Layout::UpdateLayout() noexcept
{
	UpdateRows();
	UpdateColumns();

#ifdef _DEBUG
	LayoutCheck();
#endif // _DEBUG
}
void Layout::UpdateRows() noexcept
{
	// If there are no rows, log a warning but then create a single row that takes up all the space
	if (m_rowDefinitions.size() == 0)
	{
		EG_CORE_WARN("{}:{} - Layout (name: {}) called UpdateRows, but no rows exist", __FILE__, __LINE__, m_name);
		AddRow({ RowColumnType::STAR, 1.0f });
	}

	// Get the total fixed & percent height
	float totalFixedHeight = GetTotalFixedSize(m_rowDefinitions, m_height);
	if (totalFixedHeight > m_height)
	{
		EG_CORE_ERROR("{}:{} - Layout (name: {}) has row definitions FIXED + PERCENT sizing ({}) greater than the available height", __FILE__, __LINE__, m_name, totalFixedHeight, m_height);
		EG_CORE_ERROR("    Unable to add all the rows... Removing last row and trying again");
		m_rowDefinitions.pop_back();
		UpdateRows();
	}

	float totalStarHeight = m_height - totalFixedHeight;

	// Compute the height of a single star
	float totalStars = GetTotalStars(m_rowDefinitions);
	float singleStarHeight = totalStars > 0.0f ? totalStarHeight / totalStars : 0.0f;

	// Create the new rows (use the layout left and width, but compute new top and height for each row)
	float top = m_top;
	float height = 0.0f;
	for (unsigned int iii = 0; iii < m_rowDefinitions.size(); ++iii)
	{
		switch (m_rowDefinitions[iii].Type)
		{
		case RowColumnType::FIXED:		height = m_rowDefinitions[iii].Value; break;
		case RowColumnType::PERCENT:	height = (m_rowDefinitions[iii].Value * m_height); break;
		case RowColumnType::STAR:		height = (m_rowDefinitions[iii].Value * singleStarHeight); break;
		default:
			EG_CORE_ERROR("{}:{} - Layout (name: {}) has unrecognized RowColumnType", __FILE__, __LINE__, m_name);
			continue;
		}

		if (iii >= m_rows.size())
			m_rows.emplace_back(top, m_left, m_width, height);
		else
		{
			m_rows[iii].Top(top);
			m_rows[iii].Height(height);
			m_rows[iii].Left(m_left);
			m_rows[iii].Width(m_width);
		}

		top += height;
	}

	// For each row, now update the parent layout height and star height
	for (Row& row : m_rows)
	{
		row.ParentLayoutHeight(m_height);
		row.StarHeight(singleStarHeight);
	}
}
void Layout::UpdateColumns() noexcept
{
	// If there are no columns, log a warning but then create a single column that takes up all the space
	if (m_columnDefinitions.size() == 0)
	{
		EG_CORE_WARN("{}:{} - Layout (name: {}) called UpdateColumns, but no columns exist", __FILE__, __LINE__, m_name);
		AddColumn({ RowColumnType::STAR, 1.0f });
	}

	// Get the total fixed & percent height
	float totalFixedWidth = GetTotalFixedSize(m_columnDefinitions, m_width);
	if (totalFixedWidth > m_width)
	{
		EG_CORE_ERROR("{}:{} - Layout (name: {}) has column definitions FIXED + PERCENT sizing ({}) greater than the available width", __FILE__, __LINE__, m_name, totalFixedWidth, m_width);
		EG_CORE_ERROR("    Unable to add all the column... Removing last column and trying again");
		m_columnDefinitions.pop_back();
		UpdateColumns();
	}

	float totalStarWidth = m_width - totalFixedWidth;

	// Compute the width of a single star
	float totalStars = GetTotalStars(m_columnDefinitions);
	float singleStarWidth = totalStars > 0.0f ? totalStarWidth / totalStars : 0.0f;

	// Create the new columns (use the layout top and height, but compute new left and width for each column)
	float left = m_left;
	float width = 0.0f;
	for (unsigned int iii = 0; iii < m_columnDefinitions.size(); ++iii)
	{
		switch (m_columnDefinitions[iii].Type)
		{
		case RowColumnType::FIXED:		width = m_columnDefinitions[iii].Value; break;
		case RowColumnType::PERCENT:	width = (m_columnDefinitions[iii].Value * m_width); break;
		case RowColumnType::STAR:		width = (m_columnDefinitions[iii].Value * singleStarWidth); break;
		default:
			EG_CORE_ERROR("{}:{} - Layout (name: {}) has unrecognized RowColumnType", __FILE__, __LINE__, m_name);
			continue;
		}

		if (iii >= m_columns.size())
			m_columns.emplace_back(m_top, left, width, m_height);
		else
		{
			m_columns[iii].Left(left);
			m_columns[iii].Width(width);
			m_columns[iii].Top(m_top);
			m_columns[iii].Height(m_height);
		}

		left += width;
	}

	// For each column, now update the parent layout width and star width
	for (Column& column : m_columns)
	{
		column.ParentLayoutWidth(m_width);
		column.StarWidth(singleStarWidth);
	}
}

float Layout::GetTotalFixedSize(const std::vector<RowColumnDefinition>& defs, const float totalSpace) const noexcept
{
	float sum = 0.0f;

	for (const RowColumnDefinition& def : defs)
	{
		switch (def.Type)
		{
		case RowColumnType::FIXED:	 sum += def.Value; break;
		case RowColumnType::PERCENT: sum += (def.Value * totalSpace); break;
		}
	}

	return sum;
}
float Layout::GetTotalStars(const std::vector<RowColumnDefinition>& defs) const noexcept
{
	float stars = 0.0f;

	for (const RowColumnDefinition& def : defs)
	{
		if (def.Type == RowColumnType::STAR)
			stars += def.Value;
	}

	return stars;
}

void Layout::ClearRows() noexcept
{
	m_rows.clear();
	m_rowDefinitions.clear();
}
void Layout::ClearColumns() noexcept
{
	m_columns.clear();
	m_columnDefinitions.clear();
}

void Layout::DrawBorders(DeviceResources* deviceResources) const noexcept
{
	for (const Row& row : m_rows)
	{
		deviceResources->DrawLine(row.Left(), row.Top(), row.Right(), row.Top(), Color::Red);
		deviceResources->DrawLine(row.Left(), row.Top(), row.Left(), row.Bottom(), Color::Red);
		deviceResources->DrawLine(row.Left(), row.Bottom(), row.Right(), row.Bottom(), Color::Red);
		deviceResources->DrawLine(row.Right(), row.Top(), row.Right(), row.Bottom(), Color::Red);
	}

	for (const Column& col : m_columns)
	{
		deviceResources->DrawLine(col.Left(), col.Top(), col.Right(), col.Top(), Color::Blue);
		deviceResources->DrawLine(col.Left(), col.Top(), col.Left(), col.Bottom(), Color::Blue);
		deviceResources->DrawLine(col.Left(), col.Bottom(), col.Right(), col.Bottom(), Color::Blue);
		deviceResources->DrawLine(col.Right(), col.Top(), col.Right(), col.Bottom(), Color::Blue);
	}
	








}

std::optional<unsigned int> Layout::MouseOverAdjustableColumn(float mouseX, float mouseY) const noexcept
{
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		if (m_columns[iii].RightIsAdjustable())
		{
			if (mouseY > m_columns[iii].Top() && mouseY < m_columns[iii].Bottom() &&
				mouseX > m_columns[iii].Right() - 4.0f && mouseX < m_columns[iii].Right() + 4.0f)
			{
				return iii;
			}
		}
	}

	return std::nullopt;
}
std::optional<unsigned int> Layout::MouseOverAdjustableRow(float mouseX, float mouseY) const noexcept
{
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		if (m_rows[iii].BottomIsAdjustable())
		{
			if (mouseX > m_rows[iii].Left() && mouseX < m_rows[iii].Right() &&
				mouseY > m_rows[iii].Bottom() - 4.0f && mouseY < m_rows[iii].Bottom() + 4.0f)
			{
				return iii;
			}
		}
	}

	return std::nullopt;
}

void Layout::OnResize(float width, float height) noexcept
{
	m_width = width;
	m_height = height;
	UpdateLayout();
}

void Layout::OnMouseMove(MouseMoveEvent& e) noexcept
{
	if (m_activelyAdjustingLayout)
	{
		EG_CORE_INFO("{}", "Adjusting Layout...");
	}
	else if (MouseOverAdjustableColumn(e.GetX(), e.GetY()))
	{
		EG_CORE_INFO("{}", "MouseOverAdjustableColumn -> true");
		Evergreen::SetCursor(Cursor::DOUBLE_ARROW_EW);
	}
	else if (MouseOverAdjustableRow(e.GetX(), e.GetY()))
	{
		EG_CORE_INFO("{}", "MouseOverAdjustableRow -> true");
		Evergreen::SetCursor(Cursor::DOUBLE_ARROW_NS);
	}
	else
	{
		Evergreen::SetCursor(Cursor::ARROW);
	}
}
void Layout::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{

}
void Layout::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{

}





// DEBUG ONLY ======================================================================================================

#ifdef _DEBUG
void Layout::LayoutCheck() const noexcept
{
	float errorMargin = 0.0f;

	// 1. Make sure any individual row width is not greater than the layout width
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		if (m_rows[iii].Width() > m_width + errorMargin)
			EG_CORE_ERROR("{}:{} - Layout (name: {}): row #{} has width {} which is greater than layout width {}", __FILE__, __LINE__, m_name, iii, m_rows[iii].Width(), m_width);
	}

	// 2. Make sure the sum total row height is not greater than the layout height
	float sum = 0.0f;
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
		sum += m_rows[iii].Height();

	if (sum > m_height + errorMargin)
		EG_CORE_ERROR("{}:{} - Layout (name: {}): sum total of row height ({}) is greater than the layout height ({})", __FILE__, __LINE__, m_name, sum, m_height);

	// 3. Make sure any individual column height is not greater than the layout height
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		if (m_columns[iii].Height() > m_height + errorMargin)
			EG_CORE_ERROR("{}:{} - Layout (name: {}): column #{} has height {} which is greater than layout height {}", __FILE__, __LINE__, m_name, iii, m_columns[iii].Height(), m_height);
	}

	// 4. Make sure the sum total column width is not greater than the layout width
	sum = 0.0f;
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
		sum += m_columns[iii].Width();

	if (sum > m_width + errorMargin)
		EG_CORE_ERROR("{}:{} - Layout (name: {}): sum total of column width ({}) is greater than the layout width ({})", __FILE__, __LINE__, m_name, sum, m_width);

	// 5. Make sure all rows are directly next to each other
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		if (iii + 1 < m_rows.size())
		{
			if (m_rows[iii + 1].Top() - m_rows[iii].Bottom() > errorMargin)
				EG_CORE_ERROR("{}:{} - Layout (name: {}): row #{} has bottom ({}) which is significantly different from row #{} top ({})", __FILE__, __LINE__, m_name, iii, m_rows[iii].Bottom(), iii + 1, m_rows[iii + 1].Top());
		}
	}

	// 6. Make sure all columns are directly next to each other
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		if (iii + 1 < m_columns.size())
		{
			if (m_columns[iii + 1].Left() - m_columns[iii].Right() > errorMargin)
				EG_CORE_ERROR("{}:{} - Layout (name: {}): column #{} has right ({}) which is significantly different from row #{} left ({})", __FILE__, __LINE__, m_name, iii, m_columns[iii].Right(), iii + 1, m_columns[iii + 1].Left());
		}
	}

	// 7. Make sure row max height is greater than or equal to min height
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		if (m_rows[iii].MaxHeight() < m_rows[iii].MinHeight())
			EG_CORE_ERROR("{}:{} - Layout (name: {}): row #{} has max height ({}) less than min height ({})", __FILE__, __LINE__, m_name, iii, m_rows[iii].MaxHeight(), m_rows[iii].MinHeight());
	}

	// 8. Make sure column max width is greater than or equal to min width
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		if (m_columns[iii].MaxWidth() < m_columns[iii].MinWidth())
			EG_CORE_ERROR("{}:{} - Layout (name: {}): column #{} has max width ({}) less than min width ({})", __FILE__, __LINE__, m_name, iii, m_columns[iii].MaxWidth(), m_columns[iii].MinWidth());
	}

	// 9. Make sure each row max/min height is greater/less than the current row height
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		if (m_rows[iii].MaxHeight() < m_rows[iii].Height())
			EG_CORE_ERROR("{}:{} - Layout (name: {}): row #{} has height ({}) greater than max height ({})", __FILE__, __LINE__, m_name, iii, m_rows[iii].Height(), m_rows[iii].MaxHeight());

		if (m_rows[iii].MinHeight() > m_rows[iii].Height())
			EG_CORE_ERROR("{}:{} - Layout (name: {}): row #{} has height ({}) less than min height ({})", __FILE__, __LINE__, m_name, iii, m_rows[iii].Height(), m_rows[iii].MinHeight());
	}

	// 10. Make sure each column max/min width is greater/less than the current column width
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		if (m_columns[iii].MaxWidth() < m_columns[iii].Width())
			EG_CORE_ERROR("{}:{} - Layout (name: {}): column #{} has width ({}) greater than max width ({})", __FILE__, __LINE__, m_name, iii, m_columns[iii].Width(), m_columns[iii].MaxWidth());

		if (m_columns[iii].MinWidth() > m_columns[iii].Width())
			EG_CORE_ERROR("{}:{} - Layout (name: {}): column #{} has width ({}) less than min width ({})", __FILE__, __LINE__, m_name, iii, m_columns[iii].Width(), m_columns[iii].MinWidth());
	}

	// 11. Make sure all adjacent rows have matching adjustability
	for (unsigned int iii = 0; iii < m_rows.size(); ++iii)
	{
		if (iii + 1 < m_rows.size())
		{
			if (m_rows[iii].BottomIsAdjustable() != m_rows[iii + 1].TopIsAdjustable())
				EG_CORE_ERROR("{}:{} - Layout (name: {}): row #{} bottom adjustability ({}) does not match row #{} top adjustability ({})", __FILE__, __LINE__, m_name, iii, m_rows[iii].BottomIsAdjustable() ? "true" : "false", iii + 1, m_rows[iii + 1].TopIsAdjustable() ? "true" : "false");
		}
	}
	if (m_rows[0].TopIsAdjustable())
		EG_CORE_ERROR("{}:{} - Layout (name: {}): row #0 is not allowed to be top adjustable", __FILE__, __LINE__, m_name);

	if (m_rows.back().BottomIsAdjustable())
		EG_CORE_ERROR("{}:{} - Layout (name: {}): last row is not allowed to be bottom adjustable", __FILE__, __LINE__, m_name);

	// 12. Make sure all adjacent columns have matching adjustability
	for (unsigned int iii = 0; iii < m_columns.size(); ++iii)
	{
		if (iii + 1 < m_columns.size())
		{
			if (m_columns[iii].RightIsAdjustable() != m_columns[iii + 1].LeftIsAdjustable())
				EG_CORE_ERROR("{}:{} - Layout (name: {}): column #{} right adjustability ({}) does not match row #{} left adjustability ({})", __FILE__, __LINE__, m_name, iii, m_columns[iii].RightIsAdjustable() ? "true" : "false", iii + 1, m_columns[iii + 1].LeftIsAdjustable() ? "true" : "false");
		}
	}
	if (m_columns[0].LeftIsAdjustable())
		EG_CORE_ERROR("{}:{} - Layout (name: {}): column #0 is not allowed to be left adjustable", __FILE__, __LINE__, m_name);

	if (m_columns.back().RightIsAdjustable())
		EG_CORE_ERROR("{}:{} - Layout (name: {}): last column is not allowed to be right adjustable", __FILE__, __LINE__, m_name);
}
#else
void Layout::LayoutCheck() const noexcept
{
}
#endif // _DEBUG

}