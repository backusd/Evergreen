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

	inline bool IsInCategory(EventCategory category) const noexcept
	{
		return GetCategoryFlags() & category;
	}

	inline bool Handled() const noexcept { return m_handlingControl != nullptr || m_handlingLayout != nullptr; }

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

	Control* HandlingControl() const noexcept { return m_handlingControl; }
	Layout* HandlingLayout() const noexcept { return m_handlingLayout; }

protected:
	// One and ONLY one of these should be set when a control/layout is handling an event
	Control* m_handlingControl = nullptr;
	Layout*  m_handlingLayout = nullptr;
};

}