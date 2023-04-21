#pragma once
#include "pch.h"
#include "Event.h"

namespace Evergreen
{
class EVERGREEN_API RadioButtonIsCheckedChangedEvent : public Event
{
public:
	RadioButtonIsCheckedChangedEvent(bool value) noexcept : m_value(value)
	{}
	RadioButtonIsCheckedChangedEvent(const RadioButtonIsCheckedChangedEvent&) = delete;
	RadioButtonIsCheckedChangedEvent& operator=(const RadioButtonIsCheckedChangedEvent&) = delete;

	ND inline bool IsChecked() const noexcept { return m_value; }

	ND inline std::string ToString() const noexcept override { return std::format("RadioButtonIsCheckedChangedEvent: {}", m_value); }

	// Event Class Category
	ND inline virtual int GetCategoryFlags() const noexcept override { return EventCategory::None; }

	// Event class type
	ND inline static EventType GetStaticType() noexcept { return EventType::None; }
	ND inline virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND inline virtual const char* GetName() const noexcept override { return "RadioButtonIsCheckedChanged"; }

private:
	bool m_value;
};
}