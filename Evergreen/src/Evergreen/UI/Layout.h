#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/Rendering/DeviceResources.h"
#include "Evergreen/UI/Controls/Control.h"
#include "Evergreen/UI/Brushes.h"
#include "Evergreen/Exceptions/BaseException.h"
#include "Evergreen/Utils/Timer.h"


#define LAYOUT_EXCEPTION(fmt, ...) throw Evergreen::LayoutException(__LINE__, __FILE__, std::format(fmt, __VA_ARGS__))
#define LAYOUT_EXCEPTION_IF_FALSE(condition, fmt, ...) if (!(condition)) LAYOUT_EXCEPTION(fmt, __VA_ARGS__)

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
	unsigned int Row = 0;
	unsigned int Column = 0;
	unsigned int RowSpan = 1;
	unsigned int ColumnSpan = 1;
};

// Row ------------------------------------------------------------------------------
class EVERGREEN_API Row
{
public:
	Row(float top = 0.0f, float left = 0.0f, float width = 1.0f, float height = 1.0f, RowColumnDefinition maxHeightDef = { RowColumnType::FIXED, FLT_MAX }, RowColumnDefinition minHeightDef = { RowColumnType::FIXED, 1.0f }, bool topAdjustable = false, bool bottomAdjustable = false) noexcept;
	Row(const Row&) noexcept;
	Row& operator=(const Row& rhs) noexcept;

	ND inline float Top() const noexcept { return m_top; }
	ND inline float Left() const noexcept { return m_left; }
	ND inline float Right() const noexcept { return m_left + m_width; }
	ND inline float Bottom() const noexcept { return m_top + m_height; }
	ND inline float Width() const noexcept { return m_width; }
	ND inline float Height() const noexcept { return m_height; }
	ND float MaxHeight() const noexcept;
	ND float MinHeight() const noexcept;
	ND inline bool TopIsAdjustable() const noexcept { return m_topAdjustable; }
	ND inline bool BottomIsAdjustable() const noexcept { return m_bottomAdjustable; }

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
	Column(float top = 0.0f, float left = 0.0f, float width = 0.0f, float height = 0.0f, RowColumnDefinition maxWidthDef = { RowColumnType::FIXED, FLT_MAX }, RowColumnDefinition minWidthDef = { RowColumnType::FIXED, 1.0f }, bool leftAdjustable = false, bool rightAdjustable = false) noexcept;
	Column(const Column&) noexcept;
	Column& operator=(const Column& rhs) noexcept;

	ND inline float Top() const noexcept { return m_top; }
	ND inline float Left() const noexcept { return m_left; }
	ND inline float Right() const noexcept { return m_left + m_width; }
	ND inline float Bottom() const noexcept { return m_top + m_height; }
	ND inline float Width() const noexcept { return m_width; }
	ND inline float Height() const noexcept { return m_height; }
	ND float MaxWidth() const noexcept;
	ND float MinWidth() const noexcept;
	ND inline bool LeftIsAdjustable() const noexcept { return m_leftAdjustable; }
	ND inline bool RightIsAdjustable() const noexcept { return m_rightAdjustable; }

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

// LayoutException ------------------------------------------------------------------------------
class LayoutException : public BaseException
{
public:
	LayoutException(int line, const char* file, const std::string& message) noexcept;
	LayoutException(const LayoutException&) = delete;
	LayoutException& operator=(const LayoutException&) = delete;
	virtual ~LayoutException() noexcept override {}

	ND const char* what() const noexcept override;
	ND inline const char* GetType() const noexcept override;
	ND inline std::string GetErrorInfo() const noexcept;

private:
	std::string m_message;
};

// Layout ------------------------------------------------------------------------------

class UI;

// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Layout
{
public:
	Layout(std::shared_ptr<DeviceResources> deviceResources, UI* ui,
		float top, float left, float width, float height, 
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr, const std::string& name = "Unnamed") noexcept;
	Layout(const Layout&) = delete;
	Layout& operator=(const Layout&) = delete;
	virtual ~Layout() {}

	// Handlers for Event callbacks
	virtual void HandleOnResize() {}

	Layout* AddSubLayout(RowColumnPosition position, const std::string& name = "Unnamed");

	template<class T>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources, U&& ... args ) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>);

	void BackgroundBrush(std::unique_ptr<ColorBrush> backgroundBrush) noexcept;
	void BorderBrush(std::unique_ptr<ColorBrush> borderBrush) noexcept;
	inline void BorderWidth(float widthAll) noexcept { m_borderWidths.fill(widthAll); }
	inline void BorderWidth(const std::array<float, 4>& widths) noexcept { m_borderWidths = widths; }
	inline void BorderTopLeftOffsetX(float offset) noexcept { m_borderTopLeftOffsetX = offset; }
	inline void BorderTopLeftOffsetY(float offset) noexcept { m_borderTopLeftOffsetY = offset; }
	inline void BorderTopRightOffsetX(float offset) noexcept { m_borderTopRightOffsetX = offset; }
	inline void BorderTopRightOffsetY(float offset) noexcept { m_borderTopRightOffsetY = offset; }
	inline void BorderBottomLeftOffsetX(float offset) noexcept { m_borderBottomLeftOffsetX = offset; }
	inline void BorderBottomLeftOffsetY(float offset) noexcept { m_borderBottomLeftOffsetY = offset; }
	inline void BorderBottomRightOffsetX(float offset) noexcept { m_borderBottomRightOffsetX = offset; }
	inline void BorderBottomRightOffsetY(float offset) noexcept { m_borderBottomRightOffsetY = offset; }

	ND inline ColorBrush* BackgroundBrush() const noexcept { return m_backgroundBrush.get(); }
	ND inline ColorBrush* BorderBrush() const noexcept { return m_borderBrush.get(); }
	ND inline std::array<float, 4> BorderWidth() const noexcept { return m_borderWidths; }

	Row* AddRow(RowColumnDefinition definition);
	Column* AddColumn(RowColumnDefinition definition);

	inline void ClearRows() noexcept;
	inline void ClearColumns() noexcept;
	inline void ClearControls() noexcept;
	inline void ClearSubLayouts() noexcept;
	void ClearContents() noexcept;
	ND std::unique_ptr<Control> RemoveControl(unsigned int index) noexcept;
	void RemoveRow(unsigned int index) noexcept;
	void RemoveColumn(unsigned int index) noexcept;

	ND inline std::string Name() const noexcept { return m_name; }
	void Name(const std::string& name) noexcept { m_name = name; }

	ND inline const std::vector<Row>& Rows() const noexcept { return m_rows; }
	ND inline const std::vector<Column>& Columns() const noexcept { return m_columns; }

	void ID(unsigned int id) noexcept { m_id = id; }
	unsigned int ID() const noexcept { return m_id; }

	ND inline UI* GetUI() noexcept { return m_ui; }
	ND inline Control* GetControl(unsigned int index) const noexcept;
	ND inline Layout* GetSublayout(unsigned int index) const noexcept;
	ND Control* GetControlByName(const std::string& name) const noexcept;
	ND Control* GetControlByID(unsigned int id) const noexcept;
	template<class T>
	ND T* GetControlByName(const std::string& name) const noexcept requires(std::is_base_of_v<Control, T>);
	template<class T>
	ND T* GetControlByID(unsigned int id) const noexcept requires (std::is_base_of_v<Control, T>);
	ND inline bool HasChildControlWithName(const std::string& name) const noexcept { return GetControlByName(name) != nullptr; }
	ND inline bool HasChildControlWithName(unsigned int id) const noexcept { return GetControlByID(id) != nullptr; }
	ND inline Control* GetFirstControlOfType(Control::ControlType type) const noexcept;

	ND Layout* GetLayoutByName(const std::string& name) noexcept;
	ND Layout* GetLayoutByID(unsigned int id) noexcept;

	ND inline std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }

	ND inline unsigned int NumberOfControls() noexcept { return static_cast<unsigned int>(m_controls.size()); }
	ND inline unsigned int NumberOfSubLayouts() noexcept { return static_cast<unsigned int>(m_subLayouts.size()); }

	void Resize(const D2D1_RECT_F& rect) noexcept;
	void Resize(float top, float left, float width, float height) noexcept;
	void Resize(float width, float height) noexcept;

	void OnChar(CharEvent& e);
	void OnKeyPressed(KeyPressedEvent& e);
	void OnKeyReleased(KeyReleasedEvent& e);
	void OnMouseMove(MouseMoveEvent& e);
	void OnMouseScrolledVertical(MouseScrolledEvent& e);
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e);
	void OnMouseButtonPressed(MouseButtonPressedEvent& e);
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e);

	void Update(const Timer& timer);
	void Render() const;

	ND inline bool ContainsPoint(float x, float y) const noexcept { return m_left <= x && m_left + m_width >= x && m_top <= y && m_top + m_height >= y; }

	ND inline float Top() const noexcept { return m_top; }
	ND inline float Left() const noexcept { return m_left; }
	ND inline float Width() const noexcept { return m_width; }
	ND inline float Height() const noexcept { return m_height; }
	ND inline float Right() const noexcept { return m_left + m_width; }
	ND inline float Bottom() const noexcept { return m_top + m_height; }

	inline void Margin(const Evergreen::Margin& margin) noexcept { m_margin = margin; UpdateLayout(); }
	inline void Margin(float left, float top, float right, float bottom) noexcept;
	inline void MarginLeft(float left) noexcept { m_margin.Left = left; UpdateLayout(); }
	inline void MarginTop(float top) noexcept { m_margin.Top = top; UpdateLayout(); }
	inline void MarginRight(float right) noexcept { m_margin.Right = right; UpdateLayout(); }
	inline void MarginBottom(float bottom) noexcept { m_margin.Bottom = bottom; UpdateLayout(); }
	ND inline const Evergreen::Margin& Margin() const noexcept { return m_margin; }


private:
	void UpdateLayout() noexcept;
	void UpdateRows() noexcept;
	void UpdateColumns() noexcept;
	void UpdateSubLayouts() noexcept;
	void UpdateControls() noexcept;

	ND float GetTotalFixedSize(const std::vector<RowColumnDefinition>& defs, const float totalSpace) const noexcept;
	ND float GetTotalStars(const std::vector<RowColumnDefinition>& defs) const noexcept;

	ND std::optional<unsigned int> MouseOverAdjustableColumn(float mouseX, float mouseY) const noexcept;
	ND std::optional<unsigned int> MouseOverAdjustableRow(float mouseX, float mouseY) const noexcept;

	std::shared_ptr<DeviceResources> m_deviceResources;
	UI* m_ui;
	unsigned int m_id;

	std::unique_ptr<Evergreen::ColorBrush> m_backgroundBrush;
	std::unique_ptr<Evergreen::ColorBrush> m_borderBrush;
	std::array<float, 4>				   m_borderWidths;
	float								   m_borderTopLeftOffsetX;
	float								   m_borderTopLeftOffsetY;
	float								   m_borderTopRightOffsetX;
	float								   m_borderTopRightOffsetY;
	float								   m_borderBottomLeftOffsetX;
	float								   m_borderBottomLeftOffsetY;
	float								   m_borderBottomRightOffsetX;
	float								   m_borderBottomRightOffsetY;

	float							 m_top;
	float							 m_left;
	float							 m_width; 
	float							 m_height;
	std::vector<Row>				 m_rows;
	std::vector<Column>				 m_columns;
	std::vector<RowColumnDefinition> m_rowDefinitions;
	std::vector<RowColumnDefinition> m_columnDefinitions;
	std::string						 m_name;

	Evergreen::Margin				 m_margin;

	bool							m_adjustingLayout;
	std::optional<unsigned int>		m_columnIndexBeingAdjusted;
	std::optional<unsigned int>		m_rowIndexBeingAdjusted;

	std::vector<std::unique_ptr<Layout>>	m_subLayouts;
	std::vector<RowColumnPosition>			m_subLayoutPositions;

	std::vector<std::unique_ptr<Control>>	m_controls;
	std::vector<RowColumnPosition>			m_controlPositions;


// DEBUG ONLY ======================================================================================================

public:
	void LayoutCheck() const noexcept; // Don't need ifdef statment - in a Release build, the function is just empty

#ifdef _DEBUG
	void EnableLayoutCheck() noexcept { m_layoutCheckEnabled = true; }
	void DisableLayoutCheck() noexcept { m_layoutCheckEnabled = false; }
private:
	bool m_layoutCheckEnabled = true;
#endif

};
#pragma warning( pop )

template<class T>
T* Layout::CreateControl(std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>)
{
	RowColumnPosition position; // will default to row/col 0 and rowSpan/colSpan 1

	return CreateControl<T>(position, deviceResources);
}
template<class T>
T* Layout::CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>)
{
	m_controlPositions.push_back(position);

	D2D1_RECT_F rect = D2D1::RectF(
		m_columns[position.Column].Left(),
		m_rows[position.Row].Top(),
		m_columns[position.Column + position.ColumnSpan - 1].Right(),
		m_rows[position.Row + position.RowSpan - 1].Bottom()
	);

	std::unique_ptr<T> control = std::make_unique<T>(deviceResources, m_ui, rect);

	m_controls.push_back(std::move(control));
	return (T*)m_controls.back().get();
}
template<class T, class ... U>
T* Layout::CreateControl(std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>)
{
	RowColumnPosition position; // will default to row/col 0 and rowSpan/colSpan 1

	return CreateControl<T>(position, deviceResources, std::forward<U>(args)...);
}
template<class T, class ... U>
T* Layout::CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>)
{
	m_controlPositions.push_back(position);

	D2D1_RECT_F rect = D2D1::RectF(
		m_columns[position.Column].Left(),
		m_rows[position.Row].Top(),
		m_columns[position.Column + position.ColumnSpan - 1].Right(),
		m_rows[position.Row + position.RowSpan - 1].Bottom()
	);

	std::unique_ptr<T> control = std::make_unique<T>(deviceResources, m_ui, rect, std::forward<U>(args)...);

	m_controls.push_back(std::move(control));
	return (T*)m_controls.back().get();
}

template<class T>
T* Layout::GetControlByName(const std::string& name) const noexcept requires (std::is_base_of_v<Control, T>)
{
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		Control* found = control->GetControlByName(name);
		if (found != nullptr)
			return static_cast<T*>(found);
	}

	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		T* found = sublayout->GetControlByName<T>(name);
		if (found != nullptr)
			return found;
	}

	return nullptr;
}
template<class T>
T* Layout::GetControlByID(unsigned int id) const noexcept requires (std::is_base_of_v<Control, T>)
{
	for (const std::unique_ptr<Control>& control : m_controls)
	{
		Control* found = control->GetControlByID(id);
		if (found != nullptr)
			return static_cast<T*>(found);
	}

	for (const std::unique_ptr<Layout>& sublayout : m_subLayouts)
	{
		T* found = sublayout->GetControlByID<T>(id);
		if (found != nullptr)
			return found;
	}

	return nullptr;
}

} // namespace Evergreen

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