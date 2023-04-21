#pragma once
#include "pch.h"
#include "Evergreen/UI/Controls/Button.h"





class TestButton : public Evergreen::Button
{
public:
	TestButton(std::shared_ptr<Evergreen::DeviceResources> deviceResources,
				Evergreen::UI* ui,
				const D2D1_RECT_F& allowedRegion = D2D1::RectF(0.0f, 0.0f, FLT_MAX, FLT_MAX),
				std::unique_ptr<Evergreen::ColorBrush> backgroundBrush = nullptr,
				std::unique_ptr<Evergreen::ColorBrush> borderBrush = nullptr,
				const std::array<float, 4>& borderWidths = {},
				const Evergreen::Margin& margin = { 0 }) noexcept :
		Button(deviceResources, ui, allowedRegion, std::move(backgroundBrush), std::move(borderBrush), borderWidths, margin)
	{}
	TestButton(const TestButton&) noexcept = delete;
	TestButton& operator=(const TestButton&) noexcept = delete;
	virtual ~TestButton() noexcept override {}

	virtual void OnMouseEntered(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseExited(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseMoved(Evergreen::MouseMoveEvent&) override;
	virtual void OnMouseLButtonDown(Evergreen::MouseButtonPressedEvent&) override;
	virtual void OnMouseLButtonUp(Evergreen::MouseButtonReleasedEvent&) override;
	virtual void OnClick(Evergreen::MouseButtonReleasedEvent&) override;
};