#pragma once
#include "pch.h"
#include "Event.h"
#include "KeyCode.h"


namespace Evergreen
{


// CharEvent -----------------------------------------------------
class EVERGREEN_API CharEvent : public Event
{
public:
	CharEvent(char keycode, int repeatCount) noexcept : m_keycode(keycode), m_repeatCount(repeatCount) {}
	CharEvent(const CharEvent&) = delete;
	CharEvent& operator=(const CharEvent&) = delete;

	ND inline char GetKeyCode() const noexcept { return m_keycode; }
	ND inline int GetRepeatCount() const noexcept { return m_repeatCount; }

	ND std::string ToString() const noexcept override { return std::format("CharEvent: {} (repeats: {})", m_keycode, m_repeatCount); }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryKeyboard | EventCategoryInput | EventCategoryCharacter; }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::Character; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "Character"; }

protected:
	const char m_keycode;
	const int m_repeatCount;
};

// KeyEvent -----------------------------------------------------
class EVERGREEN_API KeyEvent : public Event
{
public:
	ND inline KEY_CODE GetKeyCode() const noexcept { return m_keycode; }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryKeyboard | EventCategoryInput; }

protected:
	KeyEvent(KEY_CODE keycode) noexcept : m_keycode(keycode) {}
	KeyEvent(const KeyEvent&) = delete;
	KeyEvent& operator=(const KeyEvent&) = delete;

	const KEY_CODE m_keycode;
};

// KeyPressedEvent -----------------------------------------------------
class EVERGREEN_API KeyPressedEvent : public KeyEvent
{
public:
	KeyPressedEvent(KEY_CODE keycode, int repeatCount, bool keyWasPreviouslyDown) noexcept :
		KeyEvent(keycode), m_repeatCount(repeatCount), m_keyWasPreviouslyDown(keyWasPreviouslyDown) 
	{}
	KeyPressedEvent(const KeyPressedEvent&) = delete;
	KeyPressedEvent& operator=(const KeyPressedEvent&) = delete;

	ND inline int GetRepeatCount() const noexcept { return m_repeatCount; }
	ND inline bool KeyWasPreviouslyDown() const noexcept { return m_keyWasPreviouslyDown; }

	ND std::string ToString() const noexcept override { return std::format("KeyPressedEvent: {} (repeats: {}, previously down: {})", m_keycode, m_repeatCount, m_keyWasPreviouslyDown); }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::KeyPressed; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "KeyPressed"; }

private:
	int m_repeatCount;
	bool m_keyWasPreviouslyDown;
};

// KeyReleasedEvent -----------------------------------------------------
class EVERGREEN_API KeyReleasedEvent : public KeyEvent
{
public:
	KeyReleasedEvent(KEY_CODE keycode) noexcept :
		KeyEvent(keycode) {}
	KeyReleasedEvent(const KeyReleasedEvent&) = delete;
	KeyReleasedEvent& operator=(const KeyReleasedEvent&) = delete;

	ND std::string ToString() const noexcept override { return std::format("KeyReleasedEvent: {}", m_keycode); }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::KeyReleased; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "KeyReleased"; }
};




} // end of Evergreen namespace

EVENT_FORMATTER(CharEvent)
EVENT_FORMATTER(KeyPressedEvent)
EVENT_FORMATTER(KeyReleasedEvent)
