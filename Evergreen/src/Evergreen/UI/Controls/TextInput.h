#pragma once
#include "pch.h"
#include "Text.h"
#include "Evergreen/UI/Layout.h"

// Macro for declaring a derived class
#define TEXTINPUT(name)	class name : public Evergreen::TextInput																							\
						{																																	\
						public:																																\
							name(std::shared_ptr<Evergreen::DeviceResources> deviceResources,																\
								Evergreen::UI* ui,																											\
								const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),												\
								const std::wstring& placeholderText = L"",																					\
								std::unique_ptr<Evergreen::ColorBrush> placeholderBrush = nullptr,															\
								std::unique_ptr<Evergreen::TextStyle> placeholderStyle = nullptr,															\
								std::unique_ptr<Evergreen::ColorBrush> inputTextBrush = nullptr,															\
								std::unique_ptr<Evergreen::TextStyle> inputTextStyle = nullptr,																\
								std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,															\
								std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,																\
								float borderWidth = 0.0f,																									\
								const Evergreen::Margin& margin = { 0 }) noexcept :																			\
								Evergreen::TextInput(deviceResources, ui, allowedRegion, placeholderText,													\
									std::move(placeholderBrush), std::move(placeholderStyle), std::move(inputTextBrush),									\
									std::move(inputTextStyle), std::move(backgroundBrush), std::move(borderBrush), borderWidth, margin)						\
							{}																																\
							name(const name&) noexcept = delete;																							\
							name& operator=(const name&) noexcept = delete;																					\
							virtual ~name() noexcept override {}																							\
																																							\
							virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;															\
							virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;															\
							virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;															\
							virtual void HandleOnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;											\
							virtual void HandleOnClick(Evergreen::MouseButtonReleasedEvent&) override;														\
							virtual void HandleOnEnterKey(Evergreen::CharEvent&) override;																	\
							virtual void HandleOnInputTextChanged(Evergreen::CharEvent&) override;															\
						};																																	\


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
		UI* ui,
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
	TextInput& operator=(const TextInput&) noexcept = delete;
	virtual ~TextInput() noexcept override {}

	// Event Handlers
	virtual void HandleOnMouseEntered(MouseMoveEvent&) {}
	virtual void HandleOnMouseExited(MouseMoveEvent&) {}
	virtual void HandleOnMouseMove(MouseMoveEvent&) {}
	virtual void HandleOnMouseLButtonDown(MouseButtonPressedEvent&) {}
	virtual void HandleOnClick(MouseButtonReleasedEvent&) {}
	virtual void HandleOnEnterKey(CharEvent&) {}
	virtual void HandleOnInputTextChanged(CharEvent&) {}

	ND Layout* AddRightColumnLayout(RowColumnDefinition rightColumnDefinition);
	ND inline Layout* GetRightColumnLayout() const noexcept;

	// Inherited from Control
	void Render() const override;

	ND inline virtual Layout* GetLayoutByName(const std::string& name) noexcept override { return m_layout->GetLayoutByName(name); }
	ND inline virtual Layout* GetLayoutByID(unsigned int id) noexcept override { return m_layout->GetLayoutByID(id); }
	ND virtual Control* GetControlByName(const std::string& name) noexcept override;
	ND virtual Control* GetControlByID(unsigned int id) noexcept override;

	// Event handling
	void OnChar(CharEvent& e) override;
	void OnKeyPressed(KeyPressedEvent& e) override;
	void OnKeyReleased(KeyReleasedEvent& e) override;
	void OnMouseMove(MouseMoveEvent& e) override;
	void OnMouseScrolledVertical(MouseScrolledEvent& e) override;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) override;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) override;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) override;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) override;
	
	// Get/Set Methods
	void SetPlaceholderText(const std::wstring& placeholderText) noexcept;
	void SetPlaceholderTextStyle(std::unique_ptr<TextStyle> style) noexcept;
	void SetPlaceholderTextBrush(std::unique_ptr<ColorBrush> brush) noexcept;
	void SetInputText(const std::wstring& inputText) noexcept;
	void SetInputTextStyle(std::unique_ptr<TextStyle> style) noexcept;
	void SetInputTextBrush(std::unique_ptr<ColorBrush> brush) noexcept; 
	void SetBackgroundBrush(std::unique_ptr<ColorBrush> brush) noexcept;
	void SetBorderBrush(std::unique_ptr<ColorBrush> brush) noexcept;
	inline void SetBorderWidth(float width) noexcept;
	void SetVerticalBarBrush(std::unique_ptr<ColorBrush> brush) noexcept;
	inline void SetVerticalBarWidth(float width) noexcept;
	inline void ActivateForTextInput() noexcept { m_textInputControlIsSelected = true; }

	ND inline const std::wstring& GetPlaceholderText() const noexcept { return m_placeholderText; }
	ND inline TextStyle* GetPlaceholderTextStyle() const noexcept { return m_placeholderTextStyle.get(); }
	ND inline const ColorBrush* GetPlaceholderTextBrush() const noexcept { return m_placeholderTextBrush.get(); }
	ND inline const std::wstring& GetInputText() const noexcept { return m_inputText; }
	ND inline TextStyle* GetInputTextStyle() const noexcept { return m_inputTextStyle.get(); }
	ND inline const ColorBrush* GetInputTextBrush() const noexcept { return m_inputTextBrush.get(); }
	ND inline const ColorBrush* GetBackgroundBrush() const noexcept { return m_backgroundBrush.get(); }
	ND inline const ColorBrush* GetBorderBrush() const noexcept { return m_borderBrush.get(); }
	ND inline float GetBorderWidth() const noexcept { return m_borderWidth; }
	ND inline const ColorBrush* GetVerticalBarBrush() const noexcept { return m_verticalBarBrush.get(); }
	ND inline float GetVerticalBarWidth() const noexcept { return m_verticalBarWidth; }

	ND inline virtual ControlType GetControlType() const noexcept override { return ControlType::TextInput; }

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

	// Functions to replace the text string in the Text control
	void SetTextToPlaceholder() noexcept;
	void SetTextToInput() noexcept;

	ND inline bool ContainsPoint(float x, float y) const noexcept;

	void UpdateVerticalBar() noexcept;

	std::unique_ptr<Layout> m_layout;
	Layout* m_rightSublayout;

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
	float m_verticalBarWidth;
	bool m_drawVerticalBar;
	std::unique_ptr<ColorBrush>	m_verticalBarBrush;

	// Data for when the text string exceeds the available width
	static const float m_originalMarginLeft;
	float m_marginLeft;
	D2D1_RECT_F m_textRegionRect;
};
#pragma warning( pop )

}