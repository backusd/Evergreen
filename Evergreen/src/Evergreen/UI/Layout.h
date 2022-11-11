#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"




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

// Row ------------------------------------------------------------------------------
class EVERGREEN_API Row
{
public:
	Row(float top = 0.0f, float left = 0.0f, float width = 0.0f, float height = 0.0f, float maxHeight = FLT_MAX, float minHeight = 1.0f, bool topAdjustable = false, bool bottomAdjustable = false) noexcept;
	Row(const Row&) noexcept;
	void operator=(const Row& rhs) noexcept;

	inline float Top() const noexcept { return m_top; }
	inline float Left() const noexcept { return m_left; }
	inline float Right() const noexcept { return m_left + m_width; }
	inline float Bottom() const noexcept { return m_top + m_height; }
	inline float Width() const noexcept { return m_width; }
	inline float Height() const noexcept { return m_height; }
	inline float MaxHeight() const noexcept { return m_maxHeight; }	
	inline float MinHeight() const noexcept { return m_minHeight; }
	inline bool TopIsAdjustable() const noexcept { return m_topAdjustable; }
	inline bool BottomIsAdjustable() const noexcept { return m_bottomAdjustable; }

	void Top(float top) noexcept { m_top = top; }
	void Left(float left) noexcept { m_left = left; }
	void Width(float width) noexcept { m_width = width; }
	void Height(float height) noexcept { m_height = height; }
	void MaxHeight(float height) noexcept { m_maxHeight = height; }
	void MinHeight(float height) noexcept { m_minHeight = height; }
	void TopIsAdjustable(bool adjustable) noexcept { m_topAdjustable = adjustable; }
	void BottomIsAdjustable(bool adjustable) noexcept { m_bottomAdjustable = adjustable; }

private:
	float m_top;
	float m_left;
	float m_width;
	float m_height;
	float m_maxHeight;
	float m_minHeight;
	bool m_topAdjustable;
	bool m_bottomAdjustable;
};

// Column ------------------------------------------------------------------------------
class EVERGREEN_API Column
{
public:
	Column(float top = 0.0f, float left = 0.0f, float width = 0.0f, float height = 0.0f, float maxWidth = FLT_MAX, float minWidth = 1.0f, bool leftAdjustable = false, bool rightAdjustable = false) noexcept;
	Column(const Column&) noexcept;
	void operator=(const Column& rhs) noexcept;

	inline float Top() const noexcept { return m_top; }
	inline float Left() const noexcept { return m_left; }
	inline float Right() const noexcept { return m_left + m_width; }
	inline float Bottom() const noexcept { return m_top + m_height; }
	inline float Width() const noexcept { return m_width; }
	inline float Height() const noexcept { return m_height; }
	inline float MaxWidth() const noexcept { return m_maxWidth; }
	inline float MinWidth() const noexcept { return m_minWidth; }
	inline bool LeftIsAdjustable() const noexcept { return m_leftAdjustable; }
	inline bool RightIsAdjustable() const noexcept { return m_rightAdjustable; }

	void Top(float top) noexcept { m_top = top; }
	void Left(float left) noexcept { m_left = left; }
	void Width(float width) noexcept { m_width = width; }
	void Height(float height) noexcept { m_height = height; }
	void MaxWidth(float width) noexcept { m_maxWidth = width; }
	void MinWidth(float width) noexcept { m_minWidth = width; }
	void LeftIsAdjustable(bool adjustable) noexcept { m_leftAdjustable = adjustable; }
	void RightIsAdjustable(bool adjustable) noexcept { m_rightAdjustable = adjustable; }

private:
	float m_top;
	float m_left;
	float m_width;
	float m_height;
	float m_maxWidth;
	float m_minWidth;
	bool m_leftAdjustable;
	bool m_rightAdjustable;
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

	std::optional<Row*> AddRow(RowColumnDefinition definition) noexcept;
	std::optional<Column*> AddColumn(RowColumnDefinition definition) noexcept;

	void ClearRows() noexcept;
	void ClearColumns() noexcept;

	std::string Name() const noexcept { return m_name; }
	void Name(const std::string& name) noexcept { m_name = name; }

private:
	void UpdateLayout() noexcept;
	void UpdateRows() noexcept;
	void UpdateColumns() noexcept;

	float GetTotalFixedSize(const std::vector<RowColumnDefinition>& defs, const float totalSpace) const noexcept;
	float GetTotalStars(const std::vector<RowColumnDefinition>& defs) const noexcept;

	float							 m_top;
	float							 m_left;
	float							 m_width; 
	float							 m_height;
	std::vector<Row>				 m_rows;
	std::vector<Column>				 m_columns;
	std::vector<RowColumnDefinition> m_rowDefinitions;
	std::vector<RowColumnDefinition> m_columnDefinitions;

	std::string m_name;
};
#pragma warning( pop )


}