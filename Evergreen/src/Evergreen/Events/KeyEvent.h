#pragma once
#include "Event.h"

#include <sstream>

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
	KeyEvent(const int keycode) noexcept : m_keycode(keycode) {}

	const int m_keycode;
};

// KeyPressedEvent -----------------------------------------------------
class EVERGREEN_API KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(const int keycode, int repeatCount) noexcept :
		KeyEvent(keycode), m_repeatCount(repeatCount) {}

	inline int GetRepeatCount() const noexcept { return m_repeatCount; }

	std::string ToString() const noexcept override
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_keycode << " (" << m_repeatCount << " repeats)";
		return ss.str();
	}

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
	KeyReleasedEvent(const int keycode) noexcept :
		KeyEvent(keycode) {}

	std::string ToString() const noexcept override
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_keycode;
		return ss.str();
	}

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::KeyReleased; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "KeyReleased"; }
};




}