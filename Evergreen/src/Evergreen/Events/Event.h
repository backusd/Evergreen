#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"

#define EVENT_FORMATTER(class_type) \
	template <>																		\
	struct std::formatter<Evergreen::class_type> : std::formatter<std::string> {	\
		auto format(Evergreen::class_type& e, std::format_context& ctx) {			\
			return formatter<std::string>::format(e.ToString(), ctx);				\
		}																			\
	};																				\

namespace Evergreen
{
// forward declare Control & Layout so we can store pointer to these in Event
class Control;
class Layout;

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
public:
	virtual EventType GetEventType() const noexcept = 0;
	virtual const char* GetName() const noexcept = 0;
	virtual int GetCategoryFlags() const noexcept = 0;
	virtual std::string ToString() const noexcept { return GetName(); }

	ND inline bool IsInCategory(EventCategory category) const noexcept
	{
		return GetCategoryFlags() & category;
	}

	ND inline bool Handled() const noexcept { return m_handlingControl != nullptr || m_handlingLayout != nullptr; }

	inline void Handled(Control* control) noexcept 
	{ 
		EG_CORE_ASSERT(m_handlingControl == nullptr, "Handling control should not already be set");
		EG_CORE_ASSERT(m_handlingLayout == nullptr, "Handling layout should not already be set");
		
		m_handlingControl = control;
	}
	inline void Handled(Layout* layout) noexcept 
	{ 
		EG_CORE_ASSERT(m_handlingControl == nullptr, "Handling control should not already be set");
		EG_CORE_ASSERT(m_handlingLayout == nullptr, "Handling layout should not already be set");
		
		m_handlingLayout = layout;
	}
	inline void ClearHandles() noexcept
	{
		m_handlingControl = nullptr;
		m_handlingLayout = nullptr;
	}

	ND inline Control* HandlingControl() const noexcept { return m_ignoreHandlingControl ? nullptr : m_handlingControl; }
	ND inline Layout* HandlingLayout() const noexcept { return m_ignoreHandlingControl ? nullptr : m_handlingLayout; }

	inline void IgnoreHandlingControl(bool ignore) noexcept { m_ignoreHandlingControl = ignore; }

protected:
	// One and ONLY one of these should be set when a control/layout is handling an event
	Control* m_handlingControl = nullptr;
	Layout*  m_handlingLayout = nullptr;
	
	// There are cases when we want to register an event as "Handled", but want to force the UI to 
	// not actually track which control handled the event (For example, if a Button handles a Click event,
	// we want to register the event as handled, but if the Click callback leads to the button being no 
	// longer visible, then we don't want the UI to track this Button as the "handling control")
	bool m_ignoreHandlingControl = false;
};

}