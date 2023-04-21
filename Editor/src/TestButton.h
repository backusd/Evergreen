#pragma once
#include "pch.h"
#include "Evergreen/UI/Controls/Button.h"
#include "Evergreen/UI/Controls/SliderFloat.h"
#include "Evergreen/UI/Controls/SliderInt.h"
#include "Evergreen/UI/Controls/RadioButton.h"
#include "Evergreen/UI/Controls/TextInput.h"



#define BUTTON(name) class name : public Evergreen::Button																							\
					 {																																\
					 public:																														\
					 	name(std::shared_ptr<Evergreen::DeviceResources> deviceResources,															\
					 				Evergreen::UI* ui,																								\
					 				const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),									\
					 				std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,												\
					 				std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,													\
					 				const std::array<float, 4>& borderWidths = {},																	\
					 				const Evergreen::Margin& margin = { 0 }) noexcept :																\
					 		Evergreen::Button(deviceResources, ui, allowedRegion, std::move(backgroundBrush), std::move(borderBrush), borderWidths, margin)	\
					 	{}																															\
					 	name(const name&) noexcept = delete;																						\
					 	name& operator=(const name&) noexcept = delete;																				\
					 	virtual ~name() noexcept override {}																					    \
					 																																\
					 	virtual void OnMouseEntered(Evergreen::MouseMoveEvent&) override;															\
					 	virtual void OnMouseExited(Evergreen::MouseMoveEvent&) override;															\
					 	virtual void OnMouseMoved(Evergreen::MouseMoveEvent&) override;																\
					 	virtual void OnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;												\
					 	virtual void OnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;												\
					 	virtual void OnClick(Evergreen::MouseButtonReleasedEvent&) override;														\
					 }																																\


BUTTON(TestButton);


class TestSliderFloat : public Evergreen::SliderFloat
{
public:
	TestSliderFloat(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
					Evergreen::UI* ui,
					const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
					float minValue = 0.0f, float maxValue = 1.0f, float initialValue = 0.0f,
					const Evergreen::Margin& margin = { 0 }) noexcept :
		Evergreen::SliderFloat(deviceResources, ui, allowedRegion, minValue, maxValue, initialValue, margin)
	{}
	TestSliderFloat(const TestSliderFloat&) noexcept = delete;
	TestSliderFloat& operator=(const TestSliderFloat&) noexcept = delete;
	virtual ~TestSliderFloat() noexcept override {}

	// Event Handlers
	virtual void OnMouseEnteredCircle(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseExitedCircle(Evergreen::MouseMoveEvent&) override;
	virtual void OnBeginDragging(Evergreen::MouseButtonPressedEvent&) override;
	virtual void OnStoppedDragging(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void OnValueChanged(Evergreen::SliderFloatValueChangedEvent&) override;
};

class TestSliderInt : public Evergreen::SliderInt
{
public:
	TestSliderInt(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
					Evergreen::UI* ui,
					const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
					int minValue = 0, int maxValue = 100, int initialValue = 0,
					const Evergreen::Margin& margin = { 0 }) noexcept :
		Evergreen::SliderInt(deviceResources, ui, allowedRegion, minValue, maxValue, initialValue, margin)
	{}
	TestSliderInt(const TestSliderInt&) noexcept = delete;
	TestSliderInt& operator=(const TestSliderInt&) noexcept = delete;
	virtual ~TestSliderInt() noexcept override {}

	// Event Handlers
	virtual void OnMouseEnteredCircle(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseExitedCircle(Evergreen::MouseMoveEvent&) override;
	virtual void OnBeginDragging(Evergreen::MouseButtonPressedEvent&) override;
	virtual void OnStoppedDragging(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void OnValueChanged(Evergreen::SliderIntValueChangedEvent&) override;
};

class TestRadioButton : public Evergreen::RadioButton
{
public:
	TestRadioButton(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
					Evergreen::UI* ui,
					const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
					bool isChecked = false,
					float outerRadius = 6.0f,
					float innerRadius = 3.0f,
					std::unique_ptr<Evergreen::ColorBrush> outerBrush = nullptr,
					std::unique_ptr<Evergreen::ColorBrush> innerBrush = nullptr,
					const Evergreen::Margin& margin = { 0 }) noexcept :
		Evergreen::RadioButton(deviceResources, ui, allowedRegion, isChecked, outerRadius, innerRadius, std::move(outerBrush), std::move(innerBrush), margin)
	{}
	TestRadioButton(const TestRadioButton&) noexcept = delete;
	TestRadioButton& operator=(const TestRadioButton&) noexcept = delete;
	virtual ~TestRadioButton() noexcept override {}

	// Event Handlers
	virtual void OnMouseEntered(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseExited(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseMoved(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;
	virtual void OnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void OnIsCheckedChanged(Evergreen::RadioButtonIsCheckedChangedEvent&) override;
};

class TestTextInput : public Evergreen::TextInput
{
public:
	TestTextInput(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
					Evergreen::UI* ui,
					const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
					const std::wstring& placeholderText = L"",
					std::unique_ptr<Evergreen::ColorBrush> placeholderBrush = nullptr,
					std::unique_ptr<Evergreen::TextStyle> placeholderStyle = nullptr,
					std::unique_ptr<Evergreen::ColorBrush> inputTextBrush = nullptr,
					std::unique_ptr<Evergreen::TextStyle> inputTextStyle = nullptr,
					std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,
					std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,
					float borderWidth = 0.0f,
					const Evergreen::Margin& margin = { 0 }) noexcept :
		Evergreen::TextInput(deviceResources, ui, allowedRegion, placeholderText,
			std::move(placeholderBrush), std::move(placeholderStyle), std::move(inputTextBrush), 
			std::move(inputTextStyle), std::move(backgroundBrush), std::move(borderBrush), borderWidth, margin)
	{}
	TestTextInput(const TestTextInput&) noexcept = delete;
	TestTextInput& operator=(const TestTextInput&) noexcept = delete;
	virtual ~TestTextInput() noexcept override {}

	// Event Handlers
	virtual void OnMouseEntered(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseExited(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseMoved(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;
	virtual void OnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void OnClick(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void OnEnterKey(Evergreen::CharEvent&) override;
	virtual void OnInputTextChanged(Evergreen::CharEvent&) override;
};