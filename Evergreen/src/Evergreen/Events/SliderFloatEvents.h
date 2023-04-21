#pragma once
#include "pch.h"
#include "Event.h"

namespace Evergreen
{
	class EVERGREEN_API SliderFloatValueChangedEvent : public Event
	{
	public:
		SliderFloatValueChangedEvent(float value) noexcept : m_value(value)
		{}
		SliderFloatValueChangedEvent(const SliderFloatValueChangedEvent&) = delete;
		SliderFloatValueChangedEvent& operator=(const SliderFloatValueChangedEvent&) = delete;

		ND inline float GetValue() const noexcept { return m_value; }

		ND std::string ToString() const noexcept override { return std::format("SliderFloatValueChangedEvent: {}", m_value); }

		// Event Class Category
		ND virtual int GetCategoryFlags() const noexcept override { return EventCategory::None; }

		// Event class type
		ND static EventType GetStaticType() noexcept { return EventType::None; }
		ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
		ND virtual const char* GetName() const noexcept override { return "SliderFloatValueChanged"; }

	private:
		float m_value;
	};
}