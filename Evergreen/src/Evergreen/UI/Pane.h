#pragma once
#include "pch.h"
#include "Controls/Control.h"
#include "Layout.h"



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
		const std::string& title = "",
		bool resizeable = true,
		bool relocatable = true,
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		bool headerBar = true,
		std::unique_ptr<ColorBrush> headerBarBrush = nullptr,
		std::unique_ptr<ColorBrush> titleBrush = nullptr,
		float titleBarHeight = 20.0f
	);
	Pane(const Pane&) = delete;
	void operator=(const Pane&) = delete;
	virtual ~Pane() noexcept override {}

	void Update() noexcept override;
	void Render() const noexcept override;

	inline Row* AddRow(RowColumnDefinition definition);
	inline Column* AddColumn(RowColumnDefinition definition);

	void OnChar(CharEvent& e) noexcept override;
	void OnKeyPressed(KeyPressedEvent& e) noexcept override;
	void OnKeyReleased(KeyReleasedEvent& e) noexcept override;
	void OnMouseMove(MouseMoveEvent& e) noexcept override;

	void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept override;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept override;

	inline void SwitchMinimize() noexcept { m_minimized = !m_minimized; }
	inline void SetCornerRadius(float xAndY) noexcept { m_paneCornerRadiusX = xAndY; m_paneCornerRadiusY = xAndY; }
	inline void SetCornerRadius(float x, float y) noexcept { m_paneCornerRadiusX = x; m_paneCornerRadiusY = y; }

private:
	enum class MouseOverDraggableAreaState
	{
		NOT_OVER,
		OVER,
		DRAGGING
	};

	void InitializeLayoutWithHeaderBar();
	void InitializeLayoutWithoutHeaderBar();

	ND inline bool RectContainsPoint(const D2D1_RECT_F& rect, float x, float y) noexcept;
	ND bool RectContainsPoint(const D2D1_ROUNDED_RECT& rect, float x, float y);

	MouseOverDraggableAreaState m_mouseTitleBarState;
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
	D2D1_RECT_F m_paneRect;
	float m_paneCornerRadiusX;
	float m_paneCornerRadiusY;

	bool m_resizeable;
	bool m_relocatable;

	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	float m_borderWidth;

	std::unique_ptr<ColorBrush> m_headerBarBrush;
	std::string m_title;
	std::unique_ptr<ColorBrush> m_titleBrush;
	float m_titleBarHeight;

	bool m_minimized;
	bool m_visible;

	// Dragging 
	float m_lastMouseX;
	float m_lastMouseY;
};
#pragma warning( pop )

}