#pragma once
#include "Event.h"

#include <format>

namespace Evergreen
{
// KeyEvent -----------------------------------------------------
class EVERGREEN_API KeyEvent : public Event
{
public:
	inline int GetKeyCode() const noexcept { return m_keycode; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryKeyboard | EventCategoryInput; }

protected:
	KeyEvent(int keycode) noexcept : m_keycode(keycode) {}

	const int m_keycode;
};

// KeyPressedEvent -----------------------------------------------------
class EVERGREEN_API KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(int keycode, int repeatCount) noexcept :
		KeyEvent(keycode), m_repeatCount(repeatCount) {}

	inline int GetRepeatCount() const noexcept { return m_repeatCount; }

	std::string ToString() const noexcept override { return std::format("KeyPressedEvent: {} --> {} ({} repeats)", m_keycode, static_cast<char>(m_keycode), m_repeatCount); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::KeyPressed; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "KeyPressed"; }

private:
	int m_repeatCount;
};

// KeyReleasedEvent -----------------------------------------------------
class EVERGREEN_API KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(int keycode) noexcept :
		KeyEvent(keycode) {}

	std::string ToString() const noexcept override { return std::format("KeyReleasedEvent: {} --> {}", m_keycode, static_cast<char>(m_keycode)); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::KeyReleased; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "KeyReleased"; }
};




}