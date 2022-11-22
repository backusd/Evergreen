#pragma once
#include "pch.h"
#include "Evergreen/Core.h"

#define EVENT_FORMATTER(class_type) \
	template <>																		\
	struct std::formatter<Evergreen::class_type> : std::formatter<std::string> {	\
		auto format(Evergreen::class_type& e, std::format_context& ctx) {			\
			return formatter<std::string>::format(e.ToString(), ctx);				\
		}																			\
	};																				\

namespace Evergreen
{
enum class EventType
{
	None = 0,
	WindowCreate, WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	Character, KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMove, MouseScrolled, MouseButtonDoubleClick, MouseEnter, MouseLeave
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

	inline void Handled(bool handled) noexcept { m_handled = handled; }
	inline bool Handled() const noexcept { return m_handled; }

protected:
	bool m_handled = false;
};

}