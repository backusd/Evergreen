#pragma once
#include "pch.h"
#include "Evergreen/Core.h"


namespace Evergreen
{
enum class EventType
{
	None = 0,
	WindowCreate, WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	Character, KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};

enum EventCategory
{
	None = 0,
	EventCategoryApplication	= BIT(0),
	EventCategoryInput			= BIT(1),
	EventCategoryKeyboard		= BIT(2),
	EventCategoryCharacter		= BIT(3),
	EventCategoryMouse			= BIT(4),
	EventCategoryMouseButton	= BIT(5)
};

class EVERGREEN_API Event
{
	friend class EventDispatcher;
public:
	virtual EventType GetEventType() const noexcept = 0;
	virtual const char* GetName() const noexcept = 0;
	virtual int GetCategoryFlags() const noexcept = 0;
	virtual std::string ToString() const noexcept { return GetName(); }

	inline bool IsInCategory(EventCategory category) const noexcept
	{
		return GetCategoryFlags() & category;
	}

protected:
	bool m_handled = false;
};


class EVERGREEN_API EventDispatcher
{
	template<typename T>
	using EventFn = std::function<bool(T&)>;

public:
	EventDispatcher(Event& event) noexcept :
		m_event(event) {}

	template<typename T>
	bool Dispatch(EventFn<T> func) noexcept
	{
		if (m_event.GetEventType() == T::GetStaticType())
		{
			m_event.m_handled = func(*(T*)&m_event);
			return true;
		}
		return false;
	}

private:
	Event& m_event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.ToString();
}

}