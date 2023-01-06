#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Control.h"
#include "Evergreen/UI/Layout.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ScrollableLayout : public Control
{
public:
	ScrollableLayout(std::shared_ptr<DeviceResources> deviceResources,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		bool scrollVertical = true,
		bool scrollHorizontal = false,
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	ScrollableLayout(const ScrollableLayout& text) noexcept = delete;
	void operator=(const ScrollableLayout&) noexcept = delete;
	virtual ~ScrollableLayout() noexcept override {}

	void Update() noexcept override;
	void Render() const noexcept override;

	// Event handling
	void OnChar(CharEvent& e) noexcept override;
	void OnKeyPressed(KeyPressedEvent& e) noexcept override;
	void OnKeyReleased(KeyReleasedEvent& e) noexcept override;
	void OnMouseMove(MouseMoveEvent& e) noexcept override;
	void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept override;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept override;

	Row* AddRow(RowColumnDefinition definition);
	Column* AddColumn(RowColumnDefinition definition);

	Layout* AddSubLayout(RowColumnPosition position, const std::string& name = "Unnamed");

	template<class T>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>);


	void BackgroundBrush(std::unique_ptr<ColorBrush> backgroundBrush) noexcept { m_backgroundBrush = std::move(backgroundBrush); }
	void BorderBrush(std::unique_ptr<ColorBrush> borderBrush) noexcept { m_borderBrush = std::move(borderBrush); }
	void BorderWidth(float width) noexcept { m_borderWidth = width; }

	ND inline const D2D1_RECT_F& BackgroundRect() const noexcept { return m_backgroundRect; }
	ND inline Layout* GetLayout() const noexcept { return m_layout.get(); }
	ND inline float VerticalScrollOffset() const noexcept { return m_verticalScrollOffset; }
	ND inline float HoriztonalScrollOffset() const noexcept { return m_horizontalScrollOffset; }
	ND inline bool VerticallyScrollable() const noexcept { return m_canScrollHorizontal; }
	ND inline bool HorizontallyScrollable() const noexcept { return m_canScrollHorizontal; }

	// Scroll Bar data
	ND inline bool VerticalScrollBarEnabled() const noexcept { return m_verticalScrollBarEnabled; }
	ND inline bool HorizontalScrollBarEnabled() const noexcept { return m_horizontalScrollBarEnabled; }
	ND inline bool VerticalScrollBarHiddenWhenNotDraggin() const noexcept { return m_verticalScrollBarHiddenWhenNotDragging; }
	ND inline bool HorizontalScrollBarHiddenWhenNotDragging() const noexcept { return m_horizontalScrollBarHiddenWhenNotDragging; }
	ND inline float VerticalScrollBarWidth() const noexcept { return m_verticalScrollBarWidth; }
	ND inline float HorizontalScrollBarHeight() const noexcept { return m_horizontalScrollBarHeight; }

	void VerticalScrollBarCornerRadius(float x, float y) noexcept { m_verticalScrollBarCornerXRadius = x; m_verticalScrollBarCornerYRadius = y; }
	void VerticalScrollBarCornerRadius(float xAndY) noexcept { m_verticalScrollBarCornerXRadius = xAndY; m_verticalScrollBarCornerYRadius = xAndY; }
	void VerticalScrollBarEnabled(bool enabled) noexcept { m_verticalScrollBarEnabled = enabled; }
	void VerticalScrollBarHiddenWhenNotDraggin(bool hidden) noexcept { m_verticalScrollBarHiddenWhenNotDragging = hidden; }
	void VerticalScrollBarWidth(float width) noexcept { m_verticalScrollBarWidth = width; }

	void HorizontalScrollBarCornerRadius(float x, float y) noexcept { m_horizontalScrollBarCornerXRadius = x; m_horizontalScrollBarCornerYRadius = y; }
	void HorizontalScrollBarCornerRadius(float xAndY) noexcept { m_horizontalScrollBarCornerXRadius = xAndY; m_horizontalScrollBarCornerYRadius = xAndY; }
	void HorizontalScrollBarEnabled(bool enabled) noexcept { m_horizontalScrollBarEnabled = enabled; }
	void HorizontalScrollBarHiddenWhenNotDraggin(bool hidden) noexcept { m_horizontalScrollBarHiddenWhenNotDragging = hidden; }
	void HorizontalScrollBarHeight(float height) noexcept { m_horizontalScrollBarHeight = height; }

	void VerticalScrollBarBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_verticalScrollBarBrush = std::move(brush); }
	void VerticalScrollBarHoveredBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_verticalScrollBarBrushHovered = std::move(brush); }
	void VerticalScrollBarDraggingBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_verticalScrollBarBrushDragging = std::move(brush); }
	void VerticalScrollBarRegionBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_verticalScrollBarRegionBrush = std::move(brush); }

	void HorizontalScrollBarBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_horizontalScrollBarBrush = std::move(brush); }
	void HorizontalScrollBarHoveredBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_horizontalScrollBarBrushHovered = std::move(brush); }
	void HorizontalScrollBarDraggingBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_horizontalScrollBarBrushDragging = std::move(brush); }
	void HorizontalScrollBarRegionBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_horizontalScrollBarRegionBrush = std::move(brush); }

private:
	void ScrollableLayoutChanged();
	void VerticalScrollBarChanged() noexcept;
	void HorizontalScrollBarChanged() noexcept;
	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;
	inline bool RectContainsPoint(const D2D1_RECT_F& rect, float x, float y) noexcept; 
	bool RectContainsPoint(const D2D1_ROUNDED_RECT& rect, float x, float y);
	void IncrementVerticalScrollOffset(float delta);
	void IncrementHorizontalScrollOffset(float delta);

	std::unique_ptr<Layout>		m_layout;
	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	float						m_borderWidth;
	D2D1_RECT_F					m_backgroundRect;

	float m_horizontalScrollOffset;
	float m_verticalScrollOffset;
	const bool m_canScrollHorizontal;
	const bool m_canScrollVertical;

	// Scroll Bar data ---------------------------------
	enum class MouseOverBarState
	{
		NOT_OVER,
		OVER,
		DRAGGING
	};

	D2D1_RECT_F			m_verticalScrollBar;
	float				m_verticalScrollBarCornerXRadius;
	float				m_verticalScrollBarCornerYRadius;
	D2D1_RECT_F			m_verticalScrollBarRegion;
	float				m_verticalScrollBarWidth;
	float				m_verticalScrollBarRegionWidth;
	MouseOverBarState	m_verticalScrollBarState;

	D2D1_RECT_F			m_horizontalScrollBar;
	float				m_horizontalScrollBarCornerXRadius;
	float				m_horizontalScrollBarCornerYRadius;
	D2D1_RECT_F			m_horizontalScrollBarRegion;
	float				m_horizontalScrollBarHeight;
	float				m_horizontalScrollBarRegionHeight;
	MouseOverBarState	m_horizontalScrollBarState;

	D2D1_POINT_2F		m_dragStartPoint;

	bool m_verticalScrollBarEnabled;
	bool m_horizontalScrollBarEnabled;
	bool m_verticalScrollBarHiddenWhenNotDragging;
	bool m_horizontalScrollBarHiddenWhenNotDragging;

	std::unique_ptr<ColorBrush> m_verticalScrollBarBrush;
	std::unique_ptr<ColorBrush> m_verticalScrollBarBrushHovered;
	std::unique_ptr<ColorBrush> m_verticalScrollBarBrushDragging;
	std::unique_ptr<ColorBrush> m_verticalScrollBarRegionBrush;

	std::unique_ptr<ColorBrush> m_horizontalScrollBarBrush;
	std::unique_ptr<ColorBrush> m_horizontalScrollBarBrushHovered;
	std::unique_ptr<ColorBrush> m_horizontalScrollBarBrushDragging;
	std::unique_ptr<ColorBrush> m_horizontalScrollBarRegionBrush;

};
#pragma warning( pop )


template<class T>
T* ScrollableLayout::CreateControl(std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	return m_layout->CreateControl<T>(deviceResources);
}
template<class T>
T* ScrollableLayout::CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	return m_layout->CreateControl<T>(position, deviceResources);
}
template<class T, class ... U>
T* ScrollableLayout::CreateControl(std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	return m_layout->CreateControl<T>(deviceResources, std::forward<U>(args)...);
}
template<class T, class ... U>
T* ScrollableLayout::CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_layout != nullptr, "No layout");
	return m_layout->CreateControl<T>(position, deviceResources, std::forward<U>(args)...);
}


}