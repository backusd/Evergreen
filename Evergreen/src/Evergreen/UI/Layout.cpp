#include "pch.h"
#include "Layout.h"
//#include "Evergreen/Utils/SetCursor.h"

#include "Evergreen/Window/Window.h"

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

// LayoutException ------------------------------------------------------------------------------
LayoutException::LayoutException(int line, const char* file, const std::string& message) noexcept :
	BaseException(line, file),
	m_message(message)
{
}
const char* LayoutException::what() const noexcept
{
	m_whatBuffer = std::format("{}\n\n[Error Info]\n{}\n\n{}", GetType(), GetErrorInfo(), GetOriginString());
	return m_whatBuffer.c_str();
}
const char* LayoutException::GetType() const noexcept
{
	return "Layout Exception";
}
std::string LayoutException::GetErrorInfo() const noexcept
{
	return m_message;
}

// Layout ------------------------------------------------------------------------------
Layout::Layout(std::shared_ptr<DeviceResources> deviceResources, UI* ui, float top, float left, float width, float height, std::unique_ptr<ColorBrush> backgroundBrush, const std::string& name) noexcept :
	m_top(top), m_left(left), m_width(width), m_height(height), m_name(name),
	m_columnIndexBeingAdjusted(std::nullopt), m_rowIndexBeingAdjusted(std::nullopt),
	m_adjustingLayout(false),
	m_deviceResources(deviceResources),
	m_ui(ui),
	m_backgroundBrush(std::move(backgroundBrush))
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(ui != nullptr, "No UI");

	// Leave rows and columns empty for now
}

Row* Layout::AddRow(RowColumnDefinition definition)
{
	switch (definition.Type)
	{
	case RowColumnType::FIXED:
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value > 0.0f, "Layout (name: {}) called AddRow(), with an invalid FIXED value ({}). Must be between greater than 0", m_name, definition.Value);
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value <= m_height, "Layout (name: {}) called AddRow, with a FIXED height ({}) greater than the layout's current height ({})", m_name, definition.Value, m_height);
		break;

	case RowColumnType::PERCENT:
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value <= 1.0f && definition.Value > 0.0f, "Layout (name: {}) called AddRow, with an invalid PERCENT height ({}). Must be between [0, 1]", m_name, definition.Value);
		break;

	case RowColumnType::STAR:
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value > 0.0f, "Layout (name: {}) called AddRow, with an invalid STAR value ({}). Must be between greater than 0", m_name, definition.Value);
		break;
	}

	m_rowDefinitions.push_back(definition);
	UpdateRows();

	EG_CORE_ASSERT(m_rows.size() > 0, "Adding row failed. Should have thrown exception.");
	EG_CORE_ASSERT(&m_rows.back() != nullptr, "Row entry should never be nullptr. Should have thrown exception.");
	
	return &m_rows.back();
}
Column* Layout::AddColumn(RowColumnDefinition definition)
{
	switch (definition.Type)
	{
	case RowColumnType::FIXED:
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value > 0.0f, "Layout (name: {}) called AddColumn, with an invalid FIXED value ({}). Must be between greater than 0", m_name, definition.Value);
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value <= m_width, "Layout (name: {}) called AddColumn, with a FIXED width ({}) greater than the layout's current width ({})", m_name, definition.Value, m_width);
		break;

	case RowColumnType::PERCENT:
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value <= 1.0f && definition.Value > 0.0f, "Layout (name: {}) called AddColumn, with an invalid PERCENT height ({}). Must be between (0, 1]", m_name, definition.Value);
		break;

	case RowColumnType::STAR:
		LAYOUT_EXCEPTION_IF_FALSE(definition.Value > 0.0f, "Layout (name: {}) called AddColumn, with an invalid STAR value ({}). Must be between greater than 0", m_name, definition.Value);
		break;
	}

	m_columnDefinitions.push_back(definition);
	UpdateColumns();

	EG_CORE_ASSERT(m_columns.size() > 0, "Adding column failed. Should have thrown exception.");
	EG_CORE_ASSERT(&m_columns.back() != nullptr, "Column entry should never be nullptr. Should have thrown exception.");
	
	return &m_columns.back();
}

void Layout::BackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	EG_CORE_ASSERT(brush != nullptr, "brush cannot be nullptr");

	m_backgroundBrush = std::move(brush);
	m_backgroundBrush->SetDrawRegion(D2D1::RectF(m_left, m_top, m_left + m_width, m_top + m_height));
}
void Layout::BorderBrush(std::unique_ptr<ColorBrush> brush) noexcept
{
	m_borderBrush = std::move(brush);

	// technically we all the border brush to be nullptr so do a check here
	if (m_borderBrush != nullptr)
		m_borderBrush->SetDrawRegion(D2D1::RectF(m_left, m_top, m_left + m_width, m_top + m_height));
}

void Layout::UpdateLayout() noexcept
{
	UpdateRows();
	UpdateColumns();
	UpdateSubLayouts();
	UpdateControls();

	if (m_backgroundBrush != nullptr)
		m_backgroundBrush->SetDrawRegion(D2D1::RectF(m_left, m_top, m_left + m_width, m_top + m_height));

	if (m_borderBrush != nullptr)
		m_borderBrush->SetDrawRegion(D2D1::RectF(m_left, m_top, m_left + m_width, m_top + m_height));

#ifdef _DEBUG
	LayoutCheck();
#endif // _DEBUG
}
void Layout::UpdateRows() noexcept
{
	// If there are no rows, just return. We will work off the assumption that rows will be added later
	if (m_rowDefinitions.size() == 0)
		return;

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

	// 
	UpdateSubLayouts();
	UpdateControls();
}
void Layout::UpdateColumns() noexcept
{
	// If there are no rows, just return. We will work off the assumption that columns will be added later
	if (m_columnDefinitions.size() == 0)
		return;

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

	// 
	UpdateSubLayouts();
	UpdateControls();
}
void Layout::UpdateSubLayouts() noexcept
{
	EG_CORE_ASSERT(m_subLayouts.size() == m_subLayoutPositions.size(), std::format("{}:{} Number of sublayouts ({}) does not match number of sublayoutPositions ({}) for layout '{}'", __FILE__, __LINE__, m_subLayouts.size(), m_subLayoutPositions.size(), m_name));
	
	for (unsigned int iii = 0; iii < m_subLayouts.size(); ++iii)
	{
		m_subLayouts[iii]->m_left   = m_columns[m_subLayoutPositions[iii].Column].Left();
		m_subLayouts[iii]->m_top    = m_rows[m_subLayoutPositions[iii].Row].Top();
		m_subLayouts[iii]->m_width  = m_columns[m_subLayoutPositions[iii].Column + m_subLayoutPositions[iii].ColumnSpan - 1].Right() - m_columns[m_subLayoutPositions[iii].Column].Left();
		m_subLayouts[iii]->m_height = m_rows[m_subLayoutPositions[iii].Row + m_subLayoutPositions[iii].RowSpan - 1].Bottom() - m_rows[m_subLayoutPositions[iii].Row].Top();

		m_subLayouts[iii]->UpdateLayout();
	}
}
void Layout::UpdateControls() noexcept
{
	EG_CORE_ASSERT(m_controls.size() == m_controlPositions.size(), std::format("{}:{} Number of controls ({}) does not match number of controlPositions ({}) for layout '{}'", __FILE__, __LINE__, m_controls.size(), m_controlPositions.size(), m_name));

	for (unsigned int iii = 0; iii < m_controls.size(); ++iii)
	{
		m_controls[iii]->AllowedRegion(
			m_columns[m_controlPositions[iii].Column].Left(),
			m_rows[m_controlPositions[iii].Row].Top(),
			m_columns[m_controlPositions[iii].Column + m_controlPositions[iii].ColumnSpan - 1].Right(),
			m_rows[m_controlPositions[iii].Row + m_controlPositions[iii].RowSpan - 1].Bottom()
		);
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

void Layout::Update() noexcept
{
	for (const std::unique_ptr<Control>& control : m_controls)
		control->Update();

	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
		sublayout->Update();
}
void Layout::Render() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");
	EG_CORE_ASSERT(m_rows.size() > 0, "No rows added");
	EG_CORE_ASSERT(m_columns.size() > 0, "No columns added");

	auto context = m_deviceResources->D2DDeviceContext();

	if (m_backgroundBrush != nullptr)
		context->FillRectangle(D2D1::RectF(m_left, m_top, m_left + m_width, m_top + m_height), m_backgroundBrush->Get());

	if (m_borderBrush != nullptr && m_borderWidth > 0.0f)
	{
		context->DrawRectangle(
			D2D1::RectF(m_left, m_top, m_left + m_width, m_top + m_height),
			m_borderBrush->Get(),
			m_borderWidth
		);
	}

	DrawBorders();

	for (const std::unique_ptr<Control>& control : m_controls)
		control->Render();

	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
		sublayout->Render();
}
void Layout::DrawBorders() const noexcept
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	for (const Row& row : m_rows)
	{
		m_deviceResources->DrawLine(row.Left(), row.Top(), row.Right(), row.Top(), D2D1::ColorF(D2D1::ColorF::Red));
		m_deviceResources->DrawLine(row.Left(), row.Top(), row.Left(), row.Bottom(), D2D1::ColorF(D2D1::ColorF::Red));
		m_deviceResources->DrawLine(row.Left(), row.Bottom(), row.Right(), row.Bottom(), D2D1::ColorF(D2D1::ColorF::Red));
		m_deviceResources->DrawLine(row.Right(), row.Top(), row.Right(), row.Bottom(), D2D1::ColorF(D2D1::ColorF::Red));
	}

	for (const Column& col : m_columns)
	{
		m_deviceResources->DrawLine(col.Left(), col.Top(), col.Right(), col.Top(), D2D1::ColorF(D2D1::ColorF::Blue));
		m_deviceResources->DrawLine(col.Left(), col.Top(), col.Left(), col.Bottom(), D2D1::ColorF(D2D1::ColorF::Blue));
		m_deviceResources->DrawLine(col.Left(), col.Bottom(), col.Right(), col.Bottom(), D2D1::ColorF(D2D1::ColorF::Blue));
		m_deviceResources->DrawLine(col.Right(), col.Top(), col.Right(), col.Bottom(), D2D1::ColorF(D2D1::ColorF::Blue));
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

Control* Layout::GetControl(unsigned int index) const noexcept
{
	EG_CORE_ASSERT(index < m_controls.size(), "Index is too large/not enough controls");
	return m_controls[index].get();
}
Layout* Layout::GetSublayout(unsigned int index) const noexcept
{
	EG_CORE_ASSERT(index < m_subLayouts.size(), "Index is too large/not enough sublayouts");
	return m_subLayouts[index].get();
}
Control* Layout::GetControlByName(const std::string& name) const noexcept
{
	Control* found = nullptr;
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		found = control->GetControlByName(name);
		if (found != nullptr)
			return found;
	}

	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		found = sublayout->GetControlByName(name);
		if (found != nullptr)
			return found;
	}

	return nullptr;
}
Control* Layout::GetControlByID(unsigned int id) const noexcept
{
	Control* found = nullptr;
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		found = control->GetControlByID(id);
		if (found != nullptr)
			return found;
	}

	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		found = sublayout->GetControlByID(id);
		if (found != nullptr)
			return found;
	}

	return nullptr;
}

void Layout::Resize(const D2D1_RECT_F& rect) noexcept
{
	m_top = rect.top;
	m_left = rect.left;
	m_width = rect.right - rect.left;
	m_height = rect.bottom - rect.top;
	UpdateLayout();
}
void Layout::Resize(float top, float left, float width, float height) noexcept
{
	m_top = top;
	m_left = left;
	m_width = width;
	m_height = height;
	UpdateLayout();
}
void Layout::Resize(float width, float height) noexcept
{
	m_width = width;
	m_height = height;
	UpdateLayout();
}

void Layout::OnChar(CharEvent& e) noexcept
{
	// Layouts don't handle this event, so just pass off to child layouts/controls
	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		sublayout->OnChar(e);
		if (e.Handled())
			return;
	}
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		control->OnChar(e);
		if (e.Handled())
			return;
	}
}
void Layout::OnKeyPressed(KeyPressedEvent& e) noexcept
{
	// Layouts don't handle this event, so just pass off to child layouts/controls
	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		sublayout->OnKeyPressed(e);
		if (e.Handled())
			return;
	}
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		control->OnKeyPressed(e);
		if (e.Handled())
			return;
	}
}
void Layout::OnKeyReleased(KeyReleasedEvent& e) noexcept
{
	// Layouts don't handle this event, so just pass off to child layouts/controls
	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		sublayout->OnKeyReleased(e);
		if (e.Handled())
			return;
	}
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		control->OnKeyReleased(e);
		if (e.Handled())
			return;
	}
}
void Layout::OnMouseMove(MouseMoveEvent& e) noexcept
{
	// If adjustingLayout is true, then mouse L button is down over either a row or column
	if (m_adjustingLayout)
	{
		e.Handled(this);

		// If m_columnIndexBeingAdjusted has a value, the mouse is currently hovering over an adjustable column border
		if (m_columnIndexBeingAdjusted.has_value())
		{
			// Column is being adjusted
			float newColumnRight = 0.0f;
			unsigned int leftColumnIndex = m_columnIndexBeingAdjusted.value();
			unsigned int rightColumnIndex = leftColumnIndex + 1;

			// It should be impossible for this assert to ever trigger because the json parser does not allow the rightmost row to even be right adjustable, but its still a good check nonetheless
			EG_ASSERT(rightColumnIndex < m_columns.size(), std::format("{}:{} rightColumnIndex ({}) was greater than max m_columns index ({})", __FILE__, __LINE__, rightColumnIndex, m_columns.size() - 1));

			if (e.GetX() >= m_columns[leftColumnIndex].Right())
			{
				// Column is being dragged to the right
				// First, get the min of mouse x position and the maximum allowed column right position
				newColumnRight = std::min(e.GetX(), m_columns[leftColumnIndex].Left() + m_columns[leftColumnIndex].MaxWidth());

				// Second, get the min between the previous value and where the left side of the right column would be if the right column had its width equal to min width
				newColumnRight = std::min(newColumnRight, m_columns[rightColumnIndex].Right() - m_columns[rightColumnIndex].MinWidth());
			}
			else
			{
				// Column is being dragged to the left
				// First, get the max of mouse x position and the minimum allowed column right position
				newColumnRight = std::max(e.GetX(), m_columns[leftColumnIndex].Left() + m_columns[leftColumnIndex].MinWidth());

				// Second, get the max between the previous value and where the left side of the right column would be if the right column had its width equal to max width
				newColumnRight = std::max(newColumnRight, m_columns[rightColumnIndex].Right() - m_columns[rightColumnIndex].MaxWidth());
			}

			// Update the left column width value (left side of column will not change)
			m_columns[leftColumnIndex].Width(newColumnRight - m_columns[leftColumnIndex].Left());

			// Update both the left and width of the right column (not allowed to set the right side directly, must specify left and width)
			float secondColumnOriginalRight = m_columns[rightColumnIndex].Right();
			m_columns[rightColumnIndex].Left(m_columns[leftColumnIndex].Right());
			m_columns[rightColumnIndex].Width(secondColumnOriginalRight - m_columns[rightColumnIndex].Left());

			// Finally, update the column definition so that resizing doesn't snap the columns back to their original size
			m_columnDefinitions[leftColumnIndex].Type = RowColumnType::PERCENT;
			m_columnDefinitions[leftColumnIndex].Value = m_columns[leftColumnIndex].Width() / m_width;
			m_columnDefinitions[rightColumnIndex].Type = RowColumnType::PERCENT;
			m_columnDefinitions[rightColumnIndex].Value = m_columns[rightColumnIndex].Width() / m_width;
		}
		else if (m_rowIndexBeingAdjusted.has_value()) 	// If m_rowIndexBeingAdjusted has a value, the mouse is currently hovering over an adjustable row border
		{
			// Row is being adjusted
			float newRowBottom = 0.0f;
			unsigned int topRowIndex = m_rowIndexBeingAdjusted.value();
			unsigned int bottomRowIndex = topRowIndex + 1;

			// Its pretty much impossible for this assert to ever trigger because the json parser does not allow the bottom row to even be bottom adjustable, but its still a good check nonetheless
			EG_ASSERT(bottomRowIndex < m_rows.size(), std::format("{}:{} bottomRowIndex ({}) was greater than max m_rows index ({})", __FILE__, __LINE__, bottomRowIndex, m_rows.size() - 1));

			if (e.GetY() >= m_rows[topRowIndex].Bottom())
			{
				// Row is being dragged down
				// First, get the min of mouse y position and the maximum allowed row height
				newRowBottom = std::min(e.GetY(), m_rows[topRowIndex].Top() + m_rows[topRowIndex].MaxHeight());

				// Second, get the min between the previous value and where the top of the bottom row would be if the bottom row had its height equal to min height
				newRowBottom = std::min(newRowBottom, m_rows[bottomRowIndex].Bottom() - m_rows[bottomRowIndex].MinHeight());
			}
			else
			{
				// Row is being dragged up
				// First, get the max of mouse y position and the minimum allowed row top
				newRowBottom = std::max(e.GetY(), m_rows[topRowIndex].Top() + m_rows[topRowIndex].MinHeight());

				// Second, get the max between the previous value and where the top of the bottom row would be if the bottom row had its height equal to max height
				newRowBottom = std::max(newRowBottom, m_rows[bottomRowIndex].Bottom() - m_rows[bottomRowIndex].MaxHeight());
			}

			// Update the top row height value (top of the row will not change)
			m_rows[topRowIndex].Height(newRowBottom - m_rows[topRowIndex].Top());

			// Update both the top and height of the bottom row (not allowed to set the bottom directly, must specify top and height)
			float secondRowOriginalBottom = m_rows[bottomRowIndex].Bottom();
			m_rows[bottomRowIndex].Top(m_rows[topRowIndex].Bottom());
			m_rows[bottomRowIndex].Height(secondRowOriginalBottom - m_rows[bottomRowIndex].Top());

			// Finally, update the row definitions so that resizing doesn't snap the rows back to their original size
			m_rowDefinitions[topRowIndex].Type = RowColumnType::PERCENT;
			m_rowDefinitions[topRowIndex].Value = m_rows[topRowIndex].Height() / m_height;
			m_rowDefinitions[bottomRowIndex].Type = RowColumnType::PERCENT;
			m_rowDefinitions[bottomRowIndex].Value = m_rows[bottomRowIndex].Height() / m_height;
		}
		else
			EG_CORE_ERROR("{}:{} m_adjustingLayout = true, but no row or column index is selected which is not allowed", __FILE__, __LINE__);
	
		// 
		UpdateSubLayouts();
		UpdateControls();
	}
	else
	{
		// Not adjusting layout so pass to sublayouts and see if it gets handled
		for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
		{
			sublayout->OnMouseMove(e);
			if (e.Handled())
				return;
		}
		for (const std::unique_ptr<Control>& control : m_controls)
		{
			control->OnMouseMove(e);
			if (e.Handled())
				return;
		}

		if (m_columnIndexBeingAdjusted.has_value())
		{
			// Mouse was previously over an adjustable column border, but check to make sure that is still the case
			m_columnIndexBeingAdjusted = MouseOverAdjustableColumn(e.GetX(), e.GetY());
			if (!m_columnIndexBeingAdjusted.has_value())
				Window::SetCursor(Cursor::ARROW);
		}
		else if (m_rowIndexBeingAdjusted.has_value())
		{
			// Mouse was previously over an adjustable row border, but check to make sure that is still the case
			m_rowIndexBeingAdjusted = MouseOverAdjustableRow(e.GetX(), e.GetY());
			if (!m_rowIndexBeingAdjusted.has_value())
				Window::SetCursor(Cursor::ARROW);
		}
		else if (m_columnIndexBeingAdjusted = MouseOverAdjustableColumn(e.GetX(), e.GetY()))
		{
			// Mouse is newly over an adjustable column border -> update the cursor
			Window::SetCursor(Cursor::DOUBLE_ARROW_EW);
		}
		else if (m_rowIndexBeingAdjusted = MouseOverAdjustableRow(e.GetX(), e.GetY()))
		{
			// Mouse is newly over an adjustable row border -> update the cursor
			Window::SetCursor(Cursor::DOUBLE_ARROW_NS);
		}
	}
}
void Layout::OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept
{
	// First pass to sublayouts and see if it gets handled
	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		sublayout->OnMouseButtonPressed(e);
		if (e.Handled())
			return;
	}
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		control->OnMouseButtonPressed(e);
		if (e.Handled())
			return;
	}

	if (e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		if (m_columnIndexBeingAdjusted.has_value())
		{
			m_adjustingLayout = true;
			e.Handled(this);
		}
		else if (m_rowIndexBeingAdjusted.has_value())
		{
			m_adjustingLayout = true;
			e.Handled(this);
		}
		else
		{
			m_adjustingLayout = false;
		}
	}
}
void Layout::OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept
{
	// If we are adjusting the layout, no need to pass to sublayouts
	if (m_adjustingLayout && e.GetMouseButton() == MOUSE_BUTTON::EG_LBUTTON)
	{
		e.Handled(this);

		m_adjustingLayout = false;

		// When the button is released, it is possible that the row/column hit a max/min value
		// so that the mouse is no longer over the row/column border
		m_columnIndexBeingAdjusted = MouseOverAdjustableColumn(e.GetX(), e.GetY());
		m_rowIndexBeingAdjusted = MouseOverAdjustableRow(e.GetX(), e.GetY());

		if (!m_columnIndexBeingAdjusted.has_value() && !m_rowIndexBeingAdjusted.has_value())
			Window::SetCursor(Cursor::ARROW);
	}
	else
	{
		// Not adjusting the layout so pass to sublayouts
		for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
		{
			sublayout->OnMouseButtonReleased(e);
			if (e.Handled())
				return;
		}

		for (const std::unique_ptr<Control>& control : m_controls)
		{
			control->OnMouseButtonReleased(e);
			if (e.Handled())
				return;
		}
	}
}
void Layout::OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept
{
	// Layouts don't handle this event, so just pass off to child layouts/controls
	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		sublayout->OnMouseScrolledVertical(e);
		if (e.Handled())
			return;
	}
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		control->OnMouseScrolledVertical(e);
		if (e.Handled())
			return;
	}
}
void Layout::OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept
{
	// Layouts don't handle this event, so just pass off to child layouts/controls
	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		sublayout->OnMouseScrolledHorizontal(e);
		if (e.Handled())
			return;
	}
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		control->OnMouseScrolledHorizontal(e);
		if (e.Handled())
			return;
	}
}
void Layout::OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept
{
	// Layouts don't handle this event, so just pass off to child layouts/controls
	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		sublayout->OnMouseButtonDoubleClick(e);
		if (e.Handled())
			return;
	}
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		control->OnMouseButtonDoubleClick(e);
		if (e.Handled())
			return;
	}
}



Layout* Layout::AddSubLayout(RowColumnPosition position, const std::string& name)
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	LAYOUT_EXCEPTION_IF_FALSE(position.Row < m_rows.size(), "Failed to add sub layout '{}' to layout '{}' because RowColumnPosition.Row ({}) was greater than maximum row index ({})", name, m_name, position.Row, m_rows.size() - 1);
	LAYOUT_EXCEPTION_IF_FALSE(position.Column < m_columns.size(), "Failed to add sub layout '{}' to layout '{}' because RowColumnPosition.Column ({}) was greater than maximum column index ({})", name, m_name, position.Column, m_columns.size() - 1);
	LAYOUT_EXCEPTION_IF_FALSE(position.RowSpan > 0, "Failed to add sub layout '{}' to layout '{}'. RowColumnPosition.RowSpan must be greater than 0", name, m_name);
	LAYOUT_EXCEPTION_IF_FALSE(position.ColumnSpan > 0, "Failed to add sub layout '{}' to layout '{}'. RowColumnPosition.ColumnSpan must be greater than 0", name, m_name);
	LAYOUT_EXCEPTION_IF_FALSE(position.RowSpan <= m_rows.size() - position.Row, "Failed to add sub layout '{}' to layout '{}' because RowColumnPosition.RowSpan ({}) was greater than maximum spannable rows ({})", name, m_name, position.RowSpan, m_rows.size() - position.Row);
	LAYOUT_EXCEPTION_IF_FALSE(position.ColumnSpan <= m_columns.size() - position.Column, "Failed to add sub layout '{}' to layout '{}' because RowColumnPosition.ColumnSpan ({}) was greater than maximum spannable columns ({})", name, m_name, position.ColumnSpan, m_columns.size() - position.Column);

	m_subLayouts.push_back(
		std::make_unique<Layout>(
			m_deviceResources,
			m_ui,
			m_rows[position.Row].Top(),
			m_columns[position.Column].Left(),
			m_columns[position.Column + position.ColumnSpan - 1].Right() - m_columns[position.Column].Left(),
			m_rows[position.Row + position.RowSpan - 1].Bottom() - m_rows[position.Row].Top(),
			nullptr,
			name
		)
	);

	m_subLayoutPositions.push_back(position);

	EG_CORE_ASSERT(m_subLayouts.back() != nullptr, "Something went wrong.");

	return m_subLayouts.back().get();
}
 

// DEBUG ONLY ======================================================================================================

#ifdef _DEBUG
void Layout::LayoutCheck() const noexcept
{
	// We allow for enabling/disabling the layout check because there are times where the layout check is performed
	// before the layout is actually finalized and will produce unnecessary error messages. For example, in the ScrollableLayout,
	// a layout check will be performed after each addition. Therefore, when adding a row/column that is adjustable, you will get
	// an error saying that last row/column is adjustable which is an error.
	if (!m_layoutCheckEnabled)
		return;

	float errorMargin = 0.01f;

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
	if (m_rows.size() > 0 && m_rows[0].TopIsAdjustable())
		EG_CORE_ERROR("{}:{} - Layout (name: {}): row #0 is not allowed to be top adjustable", __FILE__, __LINE__, m_name);

	if (m_rows.size() > 0 && m_rows.back().BottomIsAdjustable())
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
	if (m_columns.size() > 0 && m_columns[0].LeftIsAdjustable())
		EG_CORE_ERROR("{}:{} - Layout (name: {}): column #0 is not allowed to be left adjustable", __FILE__, __LINE__, m_name);

	if (m_columns.size() > 0 && m_columns.back().RightIsAdjustable())
		EG_CORE_ERROR("{}:{} - Layout (name: {}): last column is not allowed to be right adjustable", __FILE__, __LINE__, m_name);
}
#else
void Layout::LayoutCheck() const noexcept
{}
#endif // _DEBUG

}