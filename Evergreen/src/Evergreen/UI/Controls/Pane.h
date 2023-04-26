#pragma once
#include "pch.h"
#include "Control.h"
#include "Button.h"
#include "../Layout.h"

// Macro for declaring a derived class
#define PANE(name)	class name : public Evergreen::Pane																										\
					{																																		\
					public:																																	\
						TestPane(std::shared_ptr<Evergreen::DeviceResources> deviceResources,																\
							Evergreen::UI* ui,																												\
							float top,																														\
							float left,																														\
							float height,																													\
							float width,																													\
							bool resizable = true,																											\
							bool relocatable = true,																										\
							std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,																\
							std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,																	\
							const std::array<float, 4>& borderWidths = {},																					\
							bool includeTitleBar = true,																									\
							std::unique_ptr<Evergreen::ColorBrush> titleBarBrush = nullptr,																	\
							float titleBarHeight = 20.0f) :																									\
							Evergreen::Pane(deviceResources, ui, top, left, height, width, resizable, relocatable, std::move(backgroundBrush),				\
								std::move(borderBrush), borderWidths, includeTitleBar, std::move(titleBarBrush), titleBarHeight)							\
						{}																																	\
						name(const name&) noexcept = delete;																								\
						name& operator=(const name&) noexcept = delete;																						\
						virtual ~name() noexcept override {}																								\
																																							\
						virtual void HandleOnMouseEnteredTitleBar(Evergreen::MouseMoveEvent&) override;														\
						virtual void HandleOnMouseExitedTitleBar(Evergreen::MouseMoveEvent&) override;														\
						virtual void HandleOnMouseEnteredContentRegion(Evergreen::MouseMoveEvent&) override;												\
						virtual void HandleOnMouseExitedContentRegion(Evergreen::MouseMoveEvent&) override;													\
						virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;																\
					};																																		\



namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Pane : public Control
{
private:
	class PaneCloseButton : public Evergreen::Button
	{
	public:
		PaneCloseButton(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
						Evergreen::UI* ui,
						const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
						std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,
						std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,
						const std::array<float, 4>& borderWidths = {},
						const Evergreen::Margin& margin = { 0 }) noexcept :
			Evergreen::Button(deviceResources, ui, allowedRegion, std::move(backgroundBrush), std::move(borderBrush), borderWidths, margin)
		{}
		PaneCloseButton(const PaneCloseButton&) noexcept = delete;
		PaneCloseButton& operator=(const PaneCloseButton&) noexcept = delete;
		virtual ~PaneCloseButton() noexcept override {}

		virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;
		virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;
		virtual void HandleOnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;
		virtual void HandleOnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;
		virtual void HandleOnClick(Evergreen::MouseButtonReleasedEvent&) override;

		std::function<void()> m_OnClickCallback = []() {};
	};
	class PaneMinimizeButton : public Evergreen::Button
	{
	public:
		PaneMinimizeButton(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
			Evergreen::UI* ui,
			const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
			std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,
			std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,
			const std::array<float, 4>& borderWidths = {},
			const Evergreen::Margin& margin = { 0 }) noexcept :
			Evergreen::Button(deviceResources, ui, allowedRegion, std::move(backgroundBrush), std::move(borderBrush), borderWidths, margin)
		{}
		PaneMinimizeButton(const PaneMinimizeButton&) noexcept = delete;
		PaneMinimizeButton& operator=(const PaneMinimizeButton&) noexcept = delete;
		virtual ~PaneMinimizeButton() noexcept override {}

		virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;
		virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;
		virtual void HandleOnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;
		virtual void HandleOnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;
		virtual void HandleOnClick(Evergreen::MouseButtonReleasedEvent&) override;

		std::function<void()> m_OnClickCallback = []() {};
	};



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
		const std::array<float, 4>& borderWidths = {},
		bool includeTitleBar = true,
		std::unique_ptr<ColorBrush> titleBarBrush = nullptr,
		float titleBarHeight = 20.0f
	);
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
	Pane& operator=(const Pane&) = delete;
	virtual ~Pane() noexcept override {}

	virtual void HandleOnMouseEnteredTitleBar(MouseMoveEvent&) {}
	virtual void HandleOnMouseExitedTitleBar(MouseMoveEvent&) {}
	virtual void HandleOnMouseEnteredContentRegion(MouseMoveEvent&) {}
	virtual void HandleOnMouseExitedContentRegion(MouseMoveEvent&) {}
	virtual void HandleOnMouseMove(MouseMoveEvent&) {}

	void OnUpdate(const Timer& timer) override;
	void Render() const override;

	// There is a special case where a Pane has handled a move event, but this Control was previously handling
	// the move events. Therefore, we need to inform this Control that the mouse is no longer over the Control
	void MouseMoveHandledByPane(MouseMoveEvent& e) override;

	ND inline virtual Layout* GetLayoutByName(const std::string& name) noexcept override { return m_contentLayout->GetLayoutByName(name); }
	ND inline virtual Layout* GetLayoutByID(unsigned int id) noexcept override { return m_contentLayout->GetLayoutByID(id); }
	ND virtual Control* GetControlByName(const std::string& name) noexcept override;
	ND virtual Control* GetControlByID(unsigned int id) noexcept override;

	inline Row* AddRow(RowColumnDefinition definition);
	inline Column* AddColumn(RowColumnDefinition definition);

	void ClearTitleBarLayoutAndAddTitle(const std::string& title, std::unique_ptr<ColorBrush> titleBrush = nullptr);

	void OnChar(CharEvent& e) override;
	void OnKeyPressed(KeyPressedEvent& e) override;
	void OnKeyReleased(KeyReleasedEvent& e) override;
	void OnMouseMove(MouseMoveEvent& e) override;

	void OnMouseScrolledVertical(MouseScrolledEvent& e) override;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) override;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) override;

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
	inline void SetBorderWidth(float widthAll) noexcept { m_borderWidths.fill(widthAll); }
	inline void SetBorderWidth(std::array<float, 4> widths) noexcept { m_borderWidths = widths; }
	inline void BorderTopLeftOffsetX(float offset) noexcept { m_borderTopLeftOffsetX = offset; }
	inline void BorderTopLeftOffsetY(float offset) noexcept { m_borderTopLeftOffsetY = offset; }
	inline void BorderTopRightOffsetX(float offset) noexcept { m_borderTopRightOffsetX = offset; }
	inline void BorderTopRightOffsetY(float offset) noexcept { m_borderTopRightOffsetY = offset; }
	inline void BorderBottomLeftOffsetX(float offset) noexcept { m_borderBottomLeftOffsetX = offset; }
	inline void BorderBottomLeftOffsetY(float offset) noexcept { m_borderBottomLeftOffsetY = offset; }
	inline void BorderBottomRightOffsetX(float offset) noexcept { m_borderBottomRightOffsetX = offset; }
	inline void BorderBottomRightOffsetY(float offset) noexcept { m_borderBottomRightOffsetY = offset; }
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
	ND inline std::array<float, 4> GetBorderWidth() const noexcept { return m_borderWidths; }
	ND inline ColorBrush* GetTitleBarBrush() const noexcept { return m_titleBarBrush.get(); }
	ND inline float GetTitleBarHeight() const noexcept { return m_titleBarHeight; }
	ND inline bool GetMinimized() const noexcept { return m_minimized; }
	ND inline bool GetVisible() const noexcept { return m_visible; }

	template<class T>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>);
	template<class T, class ... U>
	T* CreateControl(const RowColumnPosition& position, std::shared_ptr<DeviceResources> deviceResources, U&& ... args) noexcept requires (std::is_base_of_v<Control, T>);

	Layout* AddSubLayout(RowColumnPosition position, const std::string& name = "Unnamed") { return m_contentLayout->AddSubLayout(position, name); }

	ND virtual bool ContainsPoint(float x, float y) const;

	virtual ControlType GetControlType() const noexcept override { return ControlType::Pane; }

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

	ND inline bool RectContainsPoint(const D2D1_RECT_F& rect, float x, float y) const noexcept;
	ND bool RectContainsPoint(const D2D1_ROUNDED_RECT& rect, float x, float y) const;

	void ForceMouseToBeNotOverTitleAndContent(MouseMoveEvent& e) noexcept;

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
	std::array<float, 4>		m_borderWidths;
	float m_borderTopLeftOffsetX;
	float m_borderTopLeftOffsetY;
	float m_borderTopRightOffsetX;
	float m_borderTopRightOffsetY;
	float m_borderBottomLeftOffsetX;
	float m_borderBottomLeftOffsetY;
	float m_borderBottomRightOffsetX;
	float m_borderBottomRightOffsetY;

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