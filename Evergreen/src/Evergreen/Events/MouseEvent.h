#pragma once
#include "pch.h"
#include "Event.h"
#include "MouseButton.h"

namespace Evergreen
{
// Mouse Moved Event -----------------------------------------------------------
class EVERGREEN_API MouseMoveEvent : public Event
{
public:
	MouseMoveEvent(float x, float y) noexcept :
		m_mouseX(x), m_mouseY(y) {}
	MouseMoveEvent(const MouseMoveEvent&) = delete;
	void operator=(const MouseMoveEvent&) = delete;

	inline float GetX() const noexcept { return m_mouseX; }
	inline float GetY() const noexcept { return m_mouseY; }

	std::string ToString() const noexcept override { return std::format("MouseMoveEvent: ({}, {})", m_mouseX, m_mouseY); }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseMove; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseMove"; }

private:
	float m_mouseX;
	float m_mouseY;
};

// Mouse Enter Event -----------------------------------------------------------
class EVERGREEN_API MouseEnterEvent : public Event
{
public:
	MouseEnterEvent() noexcept {}
	MouseEnterEvent(const MouseEnterEvent&) = delete;
	void operator=(const MouseEnterEvent&) = delete;

	std::string ToString() const noexcept override { return "MouseEnterEvent"; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseEnter; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseEnter"; }
};

// Mouse Leave Event -----------------------------------------------------------
class EVERGREEN_API MouseLeaveEvent : public Event
{
public:
	MouseLeaveEvent() noexcept {}
	MouseLeaveEvent(const MouseLeaveEvent&) = delete;
	void operator=(const MouseLeaveEvent&) = delete;

	std::string ToString() const noexcept override { return "MouseLeaveEvent"; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseLeave; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseLeave"; }
};

// Mouse Scrolled Event -----------------------------------------------------------
class EVERGREEN_API MouseScrolledEvent : public Event
{
public:
	MouseScrolledEvent(float xOffset, float yOffset, int delta) noexcept :
		m_xOffset(xOffset), m_yOffset(yOffset), m_scrollDelta(delta)
	{}
	MouseScrolledEvent(const MouseScrolledEvent&) = delete;
	void operator=(const MouseScrolledEvent&) = delete;

	inline float GetX() const noexcept { return m_xOffset; }
	inline float GetY() const noexcept { return m_yOffset; }
	inline int GetScrollDelta() const noexcept { return m_scrollDelta; }

	std::string ToString() const noexcept override { return std::format("MouseScrolledEvent - scroll: {} ({}, {})", m_scrollDelta, m_xOffset, m_yOffset); }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseScrolled; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseScrolled"; }

private:
	float m_xOffset;
	float m_yOffset;
	int m_scrollDelta;
};

// Mouse Button Event -----------------------------------------------------------
class EVERGREEN_API MouseButtonEvent : public Event
{
public:
	inline MOUSE_BUTTON GetMouseButton() const noexcept { return m_button; }

	inline float GetX() const noexcept { return m_xOffset; }
	inline float GetY() const noexcept { return m_yOffset; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput; }

protected:
	MouseButtonEvent(MOUSE_BUTTON button, float x, float y) noexcept :
		m_button(button), m_xOffset(x), m_yOffset(y)
	{}
	MouseButtonEvent(const MouseButtonEvent&) = delete;
	void operator=(const MouseButtonEvent&) = delete;

	MOUSE_BUTTON m_button;
	float m_xOffset;
	float m_yOffset;
};

// Mouse Button Pressed Event -----------------------------------------------------------
class EVERGREEN_API MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(MOUSE_BUTTON button, float x, float y) noexcept :
		MouseButtonEvent(button, x, y) {}
	MouseButtonPressedEvent(const MouseButtonPressedEvent&) = delete;
	void operator=(const MouseButtonPressedEvent&) = delete;

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
	MouseButtonReleasedEvent(MOUSE_BUTTON button, float x, float y) noexcept :
		MouseButtonEvent(button, x, y) {}
	MouseButtonReleasedEvent(const MouseButtonReleasedEvent&) = delete;
	void operator=(const MouseButtonReleasedEvent&) = delete;

	std::string ToString() const noexcept override { return std::format("MouseButtonReleasedEvent: {}", m_button); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseButtonReleased; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseButtonReleased"; }
};

// Mouse Button Double Click Event -----------------------------------------------------------
class EVERGREEN_API MouseButtonDoubleClickEvent : public MouseButtonEvent
{
public:
	MouseButtonDoubleClickEvent(MOUSE_BUTTON button, float x, float y) noexcept :
		MouseButtonEvent(button, x, y) {}
	MouseButtonDoubleClickEvent(const MouseButtonDoubleClickEvent&) = delete;
	void operator=(const MouseButtonDoubleClickEvent&) = delete;

	std::string ToString() const noexcept override { return std::format("MouseButtonDoubleClickEvent: {}", m_button); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::MouseButtonDoubleClick; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "MouseButtonDoubleClick"; }
};

}


EVENT_FORMATTER(MouseMoveEvent)
EVENT_FORMATTER(MouseEnterEvent)
EVENT_FORMATTER(MouseLeaveEvent)
EVENT_FORMATTER(MouseScrolledEvent)
EVENT_FORMATTER(MouseButtonPressedEvent)
EVENT_FORMATTER(MouseButtonReleasedEvent)
EVENT_FORMATTER(MouseButtonDoubleClickEvent)
