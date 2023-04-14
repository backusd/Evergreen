#include "pch.h"
#include "Rectangle.h"


namespace Evergreen
{
Rectangle::Rectangle(std::shared_ptr<DeviceResources> deviceResources,
						UI* ui,
						const D2D1_RECT_F& allowedRegion,
						std::unique_ptr<ColorBrush> brush,
						float height,
						float width,
						const Evergreen::Margin& margin) noexcept :
	Control(deviceResources, ui, allowedRegion, margin),
	m_brush(std::move(brush)),
	m_height(height),
	m_width(width)
{
	if (m_brush == nullptr)
		m_brush = std::make_unique<Evergreen::SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black));

	RectangleChanged();
}

void Rectangle::Render() const
{
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources"); 
	EG_CORE_ASSERT(m_brush != nullptr, "No background brush");
	
	auto context = m_deviceResources->D2DDeviceContext();
	context->FillRectangle(m_backgroundRect, m_brush->Get());
}

Control* Rectangle::GetControlByName(const std::string& name) noexcept
{
	return m_name.compare(name) == 0 ? this : nullptr;
}
Control* Rectangle::GetControlByID(unsigned int id) noexcept
{
	return m_id == id ? this : nullptr;
}
void Rectangle::RectangleChanged()
{
	EG_CORE_ASSERT(m_brush != nullptr, "No background brush");

	// Update the background rect
	float centerX = m_allowedRegion.left + ((m_allowedRegion.right - m_allowedRegion.left) / 2);
	float halfWidth = m_width / 2;
	m_backgroundRect.left = centerX - halfWidth;
	m_backgroundRect.right = centerX + halfWidth;

	float centerY = m_allowedRegion.top + ((m_allowedRegion.bottom - m_allowedRegion.top) / 2.0f);
	float halfHeight = m_height / 2;
	m_backgroundRect.top = centerY - halfHeight;
	m_backgroundRect.bottom = centerY + halfHeight;

	m_brush->SetDrawRegion(m_backgroundRect);
}

}