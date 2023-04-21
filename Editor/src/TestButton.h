#pragma once
#include "pch.h"
#include "Evergreen/UI/Controls/Button.h"
#include "Evergreen/UI/Controls/SliderFloat.h"




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
					 		Button(deviceResources, ui, allowedRegion, std::move(backgroundBrush), std::move(borderBrush), borderWidths, margin)	\
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
		SliderFloat(deviceResources, ui, allowedRegion, minValue, maxValue, initialValue, margin)
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

