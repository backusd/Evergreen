#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/Rendering/DeviceResources.h"




namespace Evergreen
{
enum class EVERGREEN_API RowColumnType
{
	FIXED,
	PERCENT, // Must be between [0, 1]
	STAR
};

// RowColumnDefinition ------------------------------------------------------------------------------
struct EVERGREEN_API RowColumnDefinition
{
	RowColumnType	Type;
	float			Value;
};

// SubLayoutDefinition ------------------------------------------------------------------------------
struct EVERGREEN_API RowColumnPosition
{
	unsigned int Row;
	unsigned int Column;
	unsigned int RowSpan;
	unsigned int ColumnSpan;
};

// Row ------------------------------------------------------------------------------
class EVERGREEN_API Row
{
public:
	Row(float top = 0.0f, float left = 0.0f, float width = 1.0f, float height = 1.0f, RowColumnDefinition maxHeightDef = { RowColumnType::FIXED, FLT_MAX }, RowColumnDefinition minHeightDef = { RowColumnType::FIXED, 1.0f }, bool topAdjustable = false, bool bottomAdjustable = false) noexcept;
	Row(const Row&) noexcept;
	void operator=(const Row& rhs) noexcept;

	inline float Top() const noexcept { return m_top; }
	inline float Left() const noexcept { return m_left; }
	inline float Right() const noexcept { return m_left + m_width; }
	inline float Bottom() const noexcept { return m_top + m_height; }
	inline float Width() const noexcept { return m_width; }
	inline float Height() const noexcept { return m_height; }
	float MaxHeight() const noexcept;
	float MinHeight() const noexcept;
	inline bool TopIsAdjustable() const noexcept { return m_topAdjustable; }
	inline bool BottomIsAdjustable() const noexcept { return m_bottomAdjustable; }

	void Top(float top) noexcept { m_top = top; }
	void Left(float left) noexcept { m_left = left; }
	void Width(float width) noexcept { m_width = width; }
	void Height(float height) noexcept { m_height = height; }
	void StarHeight(float height) noexcept { m_starHeight = height; }
	void ParentLayoutHeight(float height) noexcept { m_parentLayoutHeight = height; }
	void MaxHeight(float height) noexcept { MaxHeight({ RowColumnType::FIXED, height }); }
	void MinHeight(float height) noexcept { MinHeight({ RowColumnType::FIXED, height }); }
	void MaxHeight(RowColumnDefinition def) noexcept { m_maxHeightDefinition = def; }
	void MinHeight(RowColumnDefinition def) noexcept { m_minHeightDefinition = def; }
	void TopIsAdjustable(bool adjustable) noexcept { m_topAdjustable = adjustable; }
	void BottomIsAdjustable(bool adjustable) noexcept { m_bottomAdjustable = adjustable; }

private:
	float m_top;
	float m_left;
	float m_width;
	float m_height;
	bool m_topAdjustable;
	bool m_bottomAdjustable;
	RowColumnDefinition m_maxHeightDefinition;
	RowColumnDefinition m_minHeightDefinition;

	// Information about the parent layout so we can calculate fixed min/max height values
	float m_starHeight;
	float m_parentLayoutHeight;
};

// Column ------------------------------------------------------------------------------
class EVERGREEN_API Column
{
public:
	/*
	Column(float top = 0.0f, float left = 0.0f, float width = 0.0f, float height = 0.0f, float maxWidth = FLT_MAX, float minWidth = 1.0f, bool leftAdjustable = false, bool rightAdjustable = false) noexcept;
	*/
	Column(float top = 0.0f, float left = 0.0f, float width = 0.0f, float height = 0.0f, RowColumnDefinition maxWidthDef = { RowColumnType::FIXED, FLT_MAX }, RowColumnDefinition minWidthDef = { RowColumnType::FIXED, 1.0f }, bool leftAdjustable = false, bool rightAdjustable = false) noexcept;
	Column(const Column&) noexcept;
	void operator=(const Column& rhs) noexcept;

	inline float Top() const noexcept { return m_top; }
	inline float Left() const noexcept { return m_left; }
	inline float Right() const noexcept { return m_left + m_width; }
	inline float Bottom() const noexcept { return m_top + m_height; }
	inline float Width() const noexcept { return m_width; }
	inline float Height() const noexcept { return m_height; }
	float MaxWidth() const noexcept;
	float MinWidth() const noexcept;
	inline bool LeftIsAdjustable() const noexcept { return m_leftAdjustable; }
	inline bool RightIsAdjustable() const noexcept { return m_rightAdjustable; }

	void Top(float top) noexcept { m_top = top; }
	void Left(float left) noexcept { m_left = left; }
	void Width(float width) noexcept { m_width = width; }
	void Height(float height) noexcept { m_height = height; }
	void StarWidth(float width) noexcept { m_starWidth = width; }
	void ParentLayoutWidth(float width) noexcept { m_parentLayoutWidth = width; }
	void MaxWidth(float width) noexcept { MaxWidth({ RowColumnType::FIXED, width }); }
	void MinWidth(float width) noexcept { MinWidth({ RowColumnType::FIXED, width }); }
	void MaxWidth(RowColumnDefinition def) noexcept { m_maxWidthDefinition = def; }
	void MinWidth(RowColumnDefinition def) noexcept { m_minWidthDefinition = def; }
	void LeftIsAdjustable(bool adjustable) noexcept { m_leftAdjustable = adjustable; }
	void RightIsAdjustable(bool adjustable) noexcept { m_rightAdjustable = adjustable; }

private:
	float m_top;
	float m_left;
	float m_width;
	float m_height;
	bool m_leftAdjustable;
	bool m_rightAdjustable;
	RowColumnDefinition m_maxWidthDefinition;
	RowColumnDefinition m_minWidthDefinition;

	// Information about the parent layout so we can calculate fixed min/max width values
	float m_starWidth;
	float m_parentLayoutWidth;
};

// Layout ------------------------------------------------------------------------------
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Layout
{
public:
	Layout(float top, float left, float width, float height, const std::string& name = "Unnamed") noexcept;
	Layout(const Layout&) = delete;
	void operator=(const Layout&) = delete;

	std::optional<Layout*> AddSubLayout(RowColumnPosition position, const std::string& name = "Unnamed") noexcept;

	std::optional<Row*> AddRow(RowColumnDefinition definition) noexcept;
	std::optional<Column*> AddColumn(RowColumnDefinition definition) noexcept;

	void ClearRows() noexcept;
	void ClearColumns() noexcept;

	std::string Name() const noexcept { return m_name; }
	void Name(const std::string& name) noexcept { m_name = name; }

	const std::vector<Row>& Rows() const noexcept { return m_rows; }
	const std::vector<Column>& Columns() const noexcept { return m_columns; }

	void OnResize(float width, float height) noexcept;
	void OnMouseMove(MouseMoveEvent& e) noexcept;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept;

	void DrawBorders(DeviceResources* deviceResources) const noexcept;

private:
	void UpdateLayout() noexcept;
	void UpdateRows() noexcept;
	void UpdateColumns() noexcept;

	float GetTotalFixedSize(const std::vector<RowColumnDefinition>& defs, const float totalSpace) const noexcept;
	float GetTotalStars(const std::vector<RowColumnDefinition>& defs) const noexcept;

	std::optional<unsigned int> MouseOverAdjustableColumn(float mouseX, float mouseY) const noexcept;
	std::optional<unsigned int> MouseOverAdjustableRow(float mouseX, float mouseY) const noexcept;

	float							 m_top;
	float							 m_left;
	float							 m_width; 
	float							 m_height;
	std::vector<Row>				 m_rows;
	std::vector<Column>				 m_columns;
	std::vector<RowColumnDefinition> m_rowDefinitions;
	std::vector<RowColumnDefinition> m_columnDefinitions;
	std::string						 m_name;

	bool							m_mouseLButtonIsDown;
	std::optional<unsigned int>		m_columnIndexBeingAdjusted;
	std::optional<unsigned int>		m_rowIndexBeingAdjusted;

	std::vector<std::unique_ptr<Layout>>	m_subLayouts;
	std::vector<RowColumnPosition>			m_subLayoutPositions;

// DEBUG ONLY ======================================================================================================

public:
	void LayoutCheck() const noexcept; // Don't need ifdef statment - in a Release build, the function is just empty

};
#pragma warning( pop )


}

template <>
struct std::formatter<Evergreen::RowColumnType> : std::formatter<std::string> {
	auto format(Evergreen::RowColumnType type, std::format_context& ctx) {
		std::string s = "";
		switch (type)
		{
		case Evergreen::RowColumnType::FIXED:	s = "RowColumnType::FIXED"; break;
		case Evergreen::RowColumnType::PERCENT: s = "RowColumnType::PERCENT"; break;
		case Evergreen::RowColumnType::STAR:	s = "RowColumnType::STAR"; break;
		}
		return formatter<std::string>::format(s, ctx);
	}
};

template <>
struct std::formatter<Evergreen::Row> : std::formatter<std::string> {
	auto format(const Evergreen::Row& row, std::format_context& ctx) {
		if (row.TopIsAdjustable() || row.BottomIsAdjustable())
		{
			return formatter<std::string>::format(
				std::format(
					"Row:\n\t(Left, Top): ({}, {})\n\tHeight: {}\n\tWidth: {}\n\tMax Height: {}\n\tMin Height: {}\n\tTop Adjustable: {}\n\tBottom Adjustable: {}",
					row.Left(), row.Top(), row.Height(), row.Width(), row.MaxHeight(), row.MinHeight(),
					row.TopIsAdjustable() ? "true" : "false",
					row.BottomIsAdjustable() ? "true" : "false"
				),
				ctx
			);
		}

		return formatter<std::string>::format(
			std::format(
				"Row:\n\t(Left, Top): ({}, {})\n\tHeight: {}\n\tWidth: {}\n\tTop Adjustable: {}\n\tBottom Adjustable: {}",
				row.Left(), row.Top(), row.Height(), row.Width(),
				row.TopIsAdjustable() ? "true" : "false",
				row.BottomIsAdjustable() ? "true" : "false"
			),
			ctx
		);
	}
};

template <>
struct std::formatter<Evergreen::Column> : std::formatter<std::string> {
	auto format(const Evergreen::Column& column, std::format_context& ctx) {
		if (column.LeftIsAdjustable() || column.RightIsAdjustable())
		{
			return formatter<std::string>::format(
				std::format(
					"Column:\n\t(Left, Top): ({}, {})\n\tHeight: {}\n\tWidth: {}\n\tMax Width: {}\n\tMin Width: {}\n\tLeft Adjustable: {}\n\tRight Adjustable: {}",
					column.Left(), column.Top(), column.Height(), column.Width(), column.MaxWidth(), column.MinWidth(),
					column.LeftIsAdjustable() ? "true" : "false",
					column.RightIsAdjustable() ? "true" : "false"
				),
				ctx
			);
		}

		return formatter<std::string>::format(
			std::format(
				"Column:\n\t(Left, Top): ({}, {})\n\tHeight: {}\n\tWidth: {}\n\tLeft Adjustable: {}\n\tRight Adjustable: {}",
				column.Left(), column.Top(), column.Height(), column.Width(),
				column.LeftIsAdjustable() ? "true" : "false",
				column.RightIsAdjustable() ? "true" : "false"
			),
			ctx
		);
	}
};