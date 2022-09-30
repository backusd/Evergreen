#pragma once
#include "Event.h"

#include <format>

namespace Evergreen
{
// Mouse Moved Event -----------------------------------------------------------
class EVERGREEN_API MouseMovedEvent : public Event
{
public:
	MouseMovedEvent(float x, float y) noexcept : 
		m_mouseX(x), m_mouseY(y) {}

	inline float GetX() const noexcept { return m_mouseX; }
	inline float GetY() const noexcept { return m_mouseY; }

	std::string ToString() const noexcept override { return std::format("MouseMovedEvent: ({}, {})", m_mouseX, m_mouseY); }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseMoved; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseMoved"; }

private:
	float m_mouseX;
	float m_mouseY;
};

// Mouse Scrolled Event -----------------------------------------------------------
class EVERGREEN_API MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(float xOffset, float yOffset) noexcept :
		m_xOffset(xOffset), m_yOffset(yOffset) {}

	inline float GetXOffset() const noexcept { return m_xOffset; }
	inline float GetYOffset() const noexcept { return m_yOffset; }

	std::string ToString() const noexcept override { return std::format("MouseScrolledEvent: ({}, {})", m_xOffset, m_yOffset); }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseScrolled; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseScrolled"; }

private:
	float m_xOffset;
	float m_yOffset;
};

// Mouse Button Event -----------------------------------------------------------
class EVERGREEN_API MouseButtonEvent : public Event
{
public:
	inline int GetMouseButton() const noexcept { return m_button; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

protected:
	MouseButtonEvent(int button) noexcept :
		m_button(button) {}

	int m_button;
};

// Mouse Button Pressed Event -----------------------------------------------------------
class EVERGREEN_API MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(int button) noexcept :
		MouseButtonEvent(button) {}

	std::string ToString() const noexcept override { return std::format("MouseButtonPressedEvent: {}", m_button); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseButtonPressed; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseButtonPressed"; }
};

// Mouse Button Released Event -----------------------------------------------------------
class EVERGREEN_API MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(int button) noexcept :
		MouseButtonEvent(button) {}

	std::string ToString() const noexcept override { return std::format("MouseButtonReleasedEvent: {}", m_button); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseButtonReleased; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseButtonReleased"; }
};

}