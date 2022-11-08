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
	void operator=(const CharEvent&) = delete;

	inline char GetKeyCode() const noexcept { return m_keycode; }
	inline int GetRepeatCount() const noexcept { return m_repeatCount; }

	std::string ToString() const noexcept override { return std::format("CharEvent: {} (repeats: {})", m_keycode, m_repeatCount); }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryKeyboard | EventCategoryInput | EventCategoryCharacter; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::Character; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "Character"; }

protected:
	const char m_keycode;
	const int m_repeatCount;
};

// KeyEvent -----------------------------------------------------
class EVERGREEN_API KeyEvent : public Event
{
public:
	inline KEY_CODE GetKeyCode() const noexcept { return m_keycode; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryKeyboard | EventCategoryInput; }

protected:
	KeyEvent(KEY_CODE keycode) noexcept : m_keycode(keycode) {}
	KeyEvent(const KeyEvent&) = delete;
	void operator=(const KeyEvent&) = delete;

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
	void operator=(const KeyPressedEvent&) = delete;

	inline int GetRepeatCount() const noexcept { return m_repeatCount; }
	inline bool KeyWasPreviouslyDown() const noexcept { return m_keyWasPreviouslyDown; }

	std::string ToString() const noexcept override { return std::format("KeyPressedEvent: {} (repeats: {}, previously down: {})", m_keycode, m_repeatCount, m_keyWasPreviouslyDown); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::KeyPressed; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "KeyPressed"; }

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
	void operator=(const KeyReleasedEvent&) = delete;

	std::string ToString() const noexcept override { return std::format("KeyReleasedEvent: {}", m_keycode); }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::KeyReleased; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "KeyReleased"; }
};




} // end of Evergreen namespace
