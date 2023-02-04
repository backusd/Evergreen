#pragma once
#include "pch.h"
#include "Control.h"
#include "../Layout.h"


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Pane : public Control
{
public:
	Pane(std::shared_ptr<DeviceResources> deviceResources,
		UI* ui,
		float top,
		float left,
		float height,
		float width,
		bool resizable = true,
		bool relocatable = true,
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		bool includeTitleBar = true,
		std::unique_ptr<ColorBrush> titleBarBrush = nullptr,
		float titleBarHeight = 20.0f
	);
	Pane(const Pane&) = delete;
	void operator=(const Pane&) = delete;
	virtual ~Pane() noexcept override {}

	void Update() noexcept override;
	void Render() const noexcept override;

	inline Row* AddRow(RowColumnDefinition definition);
	inline Column* AddColumn(RowColumnDefinition definition);

	void ClearTitleBarLayoutAndAddTitle(const std::string& title, std::unique_ptr<ColorBrush> titleBrush = nullptr);

	void OnChar(CharEvent& e) noexcept override;
	void OnKeyPressed(KeyPressedEvent& e) noexcept override;
	void OnKeyReleased(KeyReleasedEvent& e) noexcept override;
	void OnMouseMove(MouseMoveEvent& e) noexcept override;

	void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept override;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept override;

	ND inline Layout* GetTitleBarLayout() const noexcept { return m_titleLayout->GetSublayout(0); }
	ND inline Layout* GetContentLayout() const noexcept { return m_contentLayout.get(); }

	inline void SwitchMinimize() noexcept { m_minimized = !m_minimized; }
	void SwitchVisible() noexcept;

	void SetCornerRadius(float xAndY) noexcept;
	void SetCornerRadius(float x, float y) noexcept;
	inline void SetResizable(bool resizable) noexcept { m_resizable = resizable; }
	inline void SetRelocatable(bool relocatable) noexcept { m_relocatable = relocatable; }
	void SetBackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_backgroundBrush = std::move(brush); PaneChanged(); }
	void SetBorderBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_borderBrush = std::move(brush); PaneChanged(); }
	inline void SetBorderWidth(float width) noexcept { m_borderWidth = width; }
	void SetTitleBarBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_titleBarBrush = std::move(brush); PaneChanged(); }
	void SetTitleBarHeight(float height) noexcept;
	inline void SetMinimized(bool minimized) noexcept { m_minimized = minimized; }
	inline void SetVisible(bool visible) noexcept;

	ND inline float GetCornerRadiusX() const noexcept { return m_paneCornerRadiusX; }
	ND inline float GetCornerRadiusY() const noexcept { return m_paneCornerRadiusY; }
	ND inline bool GetResizble() const noexcept { return m_resizable; }
	ND inline bool GetRelocatable() const noexcept { return m_relocatable; }
	ND inline ColorBrush* GetBackgroundBrush() const noexcept { return m_backgroundBrush.get(); }
	ND inline ColorBrush* GetBorderBrush() const noexcept { return m_borderBrush.get(); }
	ND inline float GetBorderWidth() const noexcept { return m_borderWidth; }
	ND inline ColorBrush* GetTitleBarBrush() const noexcept { return m_titleBarBrush.get(); }
	ND inline float GetTitleBarHeight() const noexcept { return m_titleBarHeight; }
	ND inline bool GetMinimized() const noexcept { return m_minimized; }
	ND inline bool GetVisible() const noexcept { return m_visible; }

	void SetOnMouseEnteredTitleBarCallback(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseEnteredTitleBar = func; }
	void SetOnMouseExitedTitleBarCallback(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseExitedTitleBar = func; }
	void SetOnMouseEnteredContentRegionCallback(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseEnteredContentRegion = func; }
	void SetOnMouseExitedContentRegionCallback(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseExitedContentRegion = func; }
	void SetOnMouseMovedCallback(std::function<void(Control*, Event& e)> func) noexcept { m_OnMouseMoved = func; }

	template<class T>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>);

	Layout* AddSubLayout(RowColumnPosition position, const std::string& name = "Unnamed") { return m_contentLayout->AddSubLayout(position, name); }

private:
	enum class MouseOverDraggableAreaState
	{
		NOT_OVER,
		OVER,
		DRAGGING
	};

	void InitializeLayoutWithTitleBar();
	void CreateTitleBarLayout();
	void InitializeLayoutWithoutTitleBar();

	void PaneChanged() noexcept;

	// React to changes made when base-class methods are called
	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;

	ND inline D2D1_RECT_F TitleRect() const noexcept { return D2D1::RectF(m_allowedRegion.left, m_allowedRegion.top, m_allowedRegion.right, m_allowedRegion.top + m_titleBarHeight); }
	ND inline D2D1_RECT_F ContentRect() const noexcept { return D2D1::RectF(m_allowedRegion.left, m_allowedRegion.top + m_titleBarHeight, m_allowedRegion.right, m_allowedRegion.bottom); }
	ND inline D2D1_RECT_F RightEdgeRect() const noexcept { return D2D1::RectF(m_allowedRegion.right - edgeSensitivity, m_allowedRegion.top, m_allowedRegion.right + edgeSensitivity, m_allowedRegion.bottom); }
	ND inline D2D1_RECT_F LeftEdgeRect() const noexcept { return D2D1::RectF(m_allowedRegion.left - edgeSensitivity, m_allowedRegion.top, m_allowedRegion.left + edgeSensitivity, m_allowedRegion.bottom); }
	ND inline D2D1_RECT_F TopEdgeRect() const noexcept { return D2D1::RectF(m_allowedRegion.left, m_allowedRegion.top - edgeSensitivity, m_allowedRegion.right, m_allowedRegion.top + edgeSensitivity); }
	ND inline D2D1_RECT_F BottomEdgeRect() const noexcept { return D2D1::RectF(m_allowedRegion.left, m_allowedRegion.bottom - edgeSensitivity, m_allowedRegion.right, m_allowedRegion.bottom + edgeSensitivity); }
	ND inline D2D1_RECT_F TopRightCornerRect() const noexcept { return D2D1::RectF(m_allowedRegion.right - edgeSensitivity, m_allowedRegion.top - edgeSensitivity, m_allowedRegion.right + edgeSensitivity, m_allowedRegion.top + edgeSensitivity); }
	ND inline D2D1_RECT_F TopLeftCornerRect() const noexcept { return D2D1::RectF(m_allowedRegion.left - edgeSensitivity, m_allowedRegion.top - edgeSensitivity, m_allowedRegion.left + edgeSensitivity, m_allowedRegion.top + edgeSensitivity); }
	ND inline D2D1_RECT_F BottomRightCornerRect() const noexcept { return D2D1::RectF(m_allowedRegion.right - edgeSensitivity, m_allowedRegion.bottom - edgeSensitivity, m_allowedRegion.right + edgeSensitivity, m_allowedRegion.bottom + edgeSensitivity); }
	ND inline D2D1_RECT_F BottomLeftCornerRect() const noexcept { return D2D1::RectF(m_allowedRegion.left - edgeSensitivity, m_allowedRegion.bottom - edgeSensitivity, m_allowedRegion.left + edgeSensitivity, m_allowedRegion.bottom + edgeSensitivity); }

	ND inline bool RectContainsPoint(const D2D1_RECT_F& rect, float x, float y) noexcept;
	ND bool RectContainsPoint(const D2D1_ROUNDED_RECT& rect, float x, float y);

	void ForceMouseToBeNotOverTitleAndContent(MouseMoveEvent& e) noexcept;

	std::function<void(Control*, Event&)> m_OnMouseEnteredTitleBar = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseExitedTitleBar = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseEnteredContentRegion = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseExitedContentRegion = [](Control*, Event&) {};
	std::function<void(Control*, Event&)> m_OnMouseMoved = [](Control*, Event&) {};
	// I can't think of use cases for these so I'm only going to implement what is currently necessary
	//std::function<void(Control*, Event&)> m_OnMouseLButtonDown = [](Control*, Event&) {};
	//std::function<void(Control*, Event&)> m_OnMouseLButtonUp = [](Control*, Event&) {};
	//std::function<void(Control*, Event&)> m_OnClick = [](Control*, Event&) {};

	MouseOverDraggableAreaState m_mouseTitleBarState;
	MouseOverDraggableAreaState m_mouseContentRegionState; // Note, the content region is not actually draggable. We only use this for OVER/NOT_OVER
	MouseOverDraggableAreaState m_mouseRightEdgeState;
	MouseOverDraggableAreaState m_mouseLeftEdgeState;
	MouseOverDraggableAreaState m_mouseTopEdgeState;
	MouseOverDraggableAreaState m_mouseBottomEdgeState;
	MouseOverDraggableAreaState m_mouseBottomRightCornerState;
	MouseOverDraggableAreaState m_mouseBottomLeftCornerState;
	MouseOverDraggableAreaState m_mouseTopRightCornerState;
	MouseOverDraggableAreaState m_mouseTopLeftCornerState;

	std::unique_ptr<Layout> m_titleLayout;
	std::unique_ptr<Layout> m_contentLayout;
	float m_paneCornerRadiusX;
	float m_paneCornerRadiusY;

	bool m_resizable;
	bool m_relocatable;

	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	float m_borderWidth;

	std::unique_ptr<ColorBrush> m_titleBarBrush;
	float m_titleBarHeight;

	bool m_minimized;
	bool m_visible;

	// Dragging 
	float m_lastMouseX;
	float m_lastMouseY;
	float m_minPaneWidth;
	float m_minPaneHeight;

	// Double click on title bar: parameters to remember what height/width to return to
	float m_heightToExpandTo;
	float m_widthToExpandTo;

	// How many pixels from an edge will we all for clicking to drag the edge
	static const float edgeSensitivity;
};
#pragma warning( pop )

template<class T>
T* Pane::CreateControl(std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");
	return m_contentLayout->CreateControl(deviceResources);
}
template<class T>
T* Pane::CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");
	return m_contentLayout->CreateControl(position, deviceResources);
}
template<class T, class ... U>
T* Pane::CreateControl(std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");
	return m_contentLayout->CreateControl<T>(deviceResources, std::forward<U>(args)...);
}
template<class T, class ... U>
T* Pane::CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>)
{
	EG_CORE_ASSERT(m_contentLayout != nullptr, "No content layout");
	return m_contentLayout->CreateControl<T>(position, deviceResources, std::forward<U>(args)...);
}

}