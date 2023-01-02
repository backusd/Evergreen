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
	ND inline int VerticalScrollOffset() const noexcept { return m_verticalScrollOffset; }
	ND inline int HoriztonalScrollOffset() const noexcept { return m_horizontalScrollOffset; }
	ND inline bool VerticallyScrollable() const noexcept { return m_canScrollHorizontal; }
	ND inline bool HorizontallyScrollable() const noexcept { return m_canScrollHorizontal; }

private:
	void ScrollableLayoutChanged();
	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;

	std::unique_ptr<Layout> m_layout;
	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	float						m_borderWidth;
	D2D1_RECT_F					m_backgroundRect;

	int m_horizontalScrollOffset;
	int m_verticalScrollOffset;
	const bool m_canScrollHorizontal;
	const bool m_canScrollVertical;

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