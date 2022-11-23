#pragma once
#include "pch.h"
#include "Control.h"
#include "Evergreen/UI/Styles/TextStyle.h"

/*
namespace Evergreen
{
class EVERGREEN_API Text : public Control
{
public:
	Text(const std::string& text = "") noexcept;
	Text(const Text& text) noexcept;
	void operator=(const Text&) noexcept;
	virtual ~Text() noexcept {}

	// Create function for returning platform specific Text object
	static Text* Create(const std::string& text = "") noexcept;


	// Inherited from Control
	virtual void Render(DeviceResources* deviceResources) const noexcept override = 0;
	virtual void OnMouseMove(MouseMoveEvent& e) noexcept override = 0;
	virtual void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept override = 0;
	virtual void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept override = 0;

	// Text specific
	const std::string& GetText() const noexcept { return m_text; }


	void SetText(const std::string& text) noexcept { m_text = text; }


protected:
	std::string					m_text;
	std::shared_ptr<TextStyle>	m_style;
	float						m_xOffset;
	float						m_yOffset;
	// Rect						m_visibleRegion;
};
}
*/