#pragma once
#include "pch.h"
#include "Evergreen/UI/Controls/Button.h"
#include "Evergreen/UI/Controls/SliderFloat.h"
#include "Evergreen/UI/Controls/SliderInt.h"
#include "Evergreen/UI/Controls/RadioButton.h"
#include "Evergreen/UI/Controls/TextInput.h"
#include "Evergreen/UI/Controls/Viewport.h"
#include "Evergreen/UI/Controls/Pane.h"



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
					 	virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;															\
					 	virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;															\
					 	virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;																\
					 	virtual void HandleOnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;												\
					 	virtual void HandleOnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;												\
					 	virtual void HandleOnClick(Evergreen::MouseButtonReleasedEvent&) override;														\
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
	virtual void HandleOnMouseEnteredCircle(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseExitedCircle(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnBeginDragging(Evergreen::MouseButtonPressedEvent&) override;
	virtual void HandleOnStoppedDragging(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void HandleOnValueChanged(Evergreen::SliderFloatValueChangedEvent&) override;
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
	virtual void HandleOnMouseEnteredCircle(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseExitedCircle(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnBeginDragging(Evergreen::MouseButtonPressedEvent&) override;
	virtual void HandleOnStoppedDragging(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void HandleOnValueChanged(Evergreen::SliderIntValueChangedEvent&) override;
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
	virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;
	virtual void HandleOnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void HandleOnIsCheckedChanged(Evergreen::RadioButtonIsCheckedChangedEvent&) override;
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
	virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;
	virtual void HandleOnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void HandleOnClick(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void HandleOnEnterKey(Evergreen::CharEvent&) override;
	virtual void HandleOnInputTextChanged(Evergreen::CharEvent&) override;
};

class TestViewport : public Evergreen::Viewport
{
public:
	TestViewport(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
				Evergreen::UI* ui,
				const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
				const Evergreen::Margin& margin = { 0 }) noexcept :
		Evergreen::Viewport(deviceResources, ui, allowedRegion, margin)
	{}
	TestViewport(const TestViewport&) noexcept = delete;
	TestViewport& operator=(const TestViewport&) noexcept = delete;
	virtual ~TestViewport() noexcept override {}

	// Event Handlers
	virtual void HandleOnChar(Evergreen::CharEvent&) override;
	virtual void HandleOnKeyPressed(Evergreen::KeyPressedEvent&) override;
	virtual void HandleOnKeyReleased(Evergreen::KeyReleasedEvent&) override;
	virtual void HandleOnMouseEntered(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseExited(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseScrolledVertical(Evergreen::MouseScrolledEvent&) override;
	virtual void HandleOnMouseScrolledHorizontal(Evergreen::MouseScrolledEvent&) override;
	virtual void HandleOnMouseButtonPressed(Evergreen::MouseButtonPressedEvent&) override;
	virtual void HandleOnMouseButtonReleased(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void HandleOnClick(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void HandleOnDoubleClick(Evergreen::MouseButtonDoubleClickEvent&) override;
	virtual void HandleOnSizeChanged(float width, float height) override;
};

class TestPane : public Evergreen::Pane
{
public:
	TestPane(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
			Evergreen::UI* ui,
			float top,
			float left,
			float height,
			float width,
			bool resizable = true,
			bool relocatable = true,
			std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,
			std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,
			const std::array<float, 4>& borderWidths = {},
			bool includeTitleBar = true,
			std::unique_ptr<Evergreen::ColorBrush> titleBarBrush = nullptr,
			float titleBarHeight = 20.0f) :
		Evergreen::Pane(deviceResources, ui, top, left, height, width, resizable, relocatable, std::move(backgroundBrush),
			std::move(borderBrush), borderWidths, includeTitleBar, std::move(titleBarBrush), titleBarHeight)
	{}
	TestPane(const TestPane&) noexcept = delete;
	TestPane& operator=(const TestPane&) noexcept = delete;
	virtual ~TestPane() noexcept override {}

	// Event Handlers
	virtual void HandleOnMouseEnteredTitleBar(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseExitedTitleBar(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseEnteredContentRegion(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseExitedContentRegion(Evergreen::MouseMoveEvent&) override;
	virtual void HandleOnMouseMove(Evergreen::MouseMoveEvent&) override;
};