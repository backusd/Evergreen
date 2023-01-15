#pragma once
#include "pch.h"
#include "Text.h"
#include "Evergreen/UI/Layout.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API TextInput : public Control
{
public:
	TextInput(std::shared_ptr<DeviceResources> deviceResources,
		const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
		const std::wstring& placeholderText = L"",
		std::unique_ptr<ColorBrush> placeholderBrush = nullptr,
		std::unique_ptr<TextStyle> placeholderStyle = nullptr,
		std::unique_ptr<ColorBrush> inputTextBrush = nullptr,
		std::unique_ptr<TextStyle> inputTextStyle = nullptr,
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		const Evergreen::Margin& margin = { 0 }) noexcept;
	TextInput(const Text& text) noexcept = delete;
	void operator=(const TextInput&) noexcept = delete;
	virtual ~TextInput() noexcept override {}

	// Inherited from Control
	void Update() noexcept override {}
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
	
	
	/*
	// Text specific
	ND inline const std::wstring& GetText() const noexcept { return m_text; }
	ND inline TextStyle* GetTextStyle() const noexcept { return m_style.get(); }
	ND inline ColorBrush* GetColorBrush() const noexcept { return m_colorBrush.get(); }

	void SetText(const std::wstring& text) noexcept { m_text = text; TextChanged(); }
	void SetTextStyle(std::unique_ptr<TextStyle> style) noexcept;
	void SetColorBrush(std::unique_ptr<ColorBrush> brush) noexcept { m_colorBrush = std::move(brush); }
	*/

private:
	enum class MouseOverState
	{
		NOT_OVER,
		OVER,
		OVER_AND_LBUTTON_DOWN,
		NOT_OVER_AND_LBUTTON_DOWN
	};

	void TextInputChanged() noexcept;

	// React to changes made when base-class methods are called
	void OnMarginChanged() override;
	void OnAllowedRegionChanged() override;

	void SetTextToPlaceholder() noexcept;
	void SetTextToInput() noexcept;

	ND inline bool ContainsPoint(float x, float y) const noexcept;

	void UpdateVerticalBar() noexcept;

	std::unique_ptr<Layout> m_layout;

	// We can only have a single Text control, which we will just modify in place to switch between the placeholder text and input text
	Text* m_text;

	// Data that will be used to update the Text control
	std::wstring m_placeholderText;
	std::wstring m_inputText;
	std::unique_ptr<ColorBrush>	m_placeholderTextBrush;
	std::unique_ptr<ColorBrush>	m_inputTextBrush;
	std::unique_ptr<TextStyle>	m_placeholderTextStyle;
	std::unique_ptr<TextStyle>	m_inputTextStyle;
	bool m_textInputControlIsSelected;
	unsigned int m_nextCharIndex;

	// Other
	MouseOverState				m_mouseState;

	// Data for drawing the background rect
	std::unique_ptr<ColorBrush>	m_backgroundBrush;
	std::unique_ptr<ColorBrush>	m_borderBrush;
	float						m_borderWidth;
	D2D1_RECT_F					m_backgroundRect;

	// Data for vertical bar
	float m_verticalBarTop;
	float m_verticalBarBottom;
	float m_verticalBarX;
	bool m_drawVerticalBar;
	std::unique_ptr<ColorBrush>	m_verticalBarBrush;


};
#pragma warning( pop )

}