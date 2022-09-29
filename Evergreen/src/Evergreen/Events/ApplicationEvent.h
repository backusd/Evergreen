#pragma once
#include "Event.h"

#include <sstream>

namespace Evergreen
{
// Window Resize Event -----------------------------------------------------------
class EVERGREEN_API WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(unsigned int width, unsigned int height) noexcept :
		m_width(width), m_height(height) {}

	inline unsigned int GetWidth() const noexcept { return m_width; }
	inline unsigned int GetHeight() const noexcept { return m_height; }

	std::string ToString() const noexcept override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << m_width << ", " << m_height;
		return ss.str();
	}

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }
	
	// Event class type
	static EventType GetStaticType() noexcept { return EventType::WindowResize; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "WindowResize"; }

private:
	const unsigned int m_width;
	const unsigned int m_height;
};

// Window Close Event -----------------------------------------------------------
class EVERGREEN_API WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() noexcept {}

	std::string ToString() const noexcept override { return "WindowCloseEvent"; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::WindowClose; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "WindowClose"; }
};

// App Tick Event -----------------------------------------------------------
class EVERGREEN_API AppTickEvent : public Event
{
public:
	AppTickEvent() noexcept {}

	std::string ToString() const noexcept override { return "AppTickEvent"; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::AppTick; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "AppTick"; }
};

// App Update Event -----------------------------------------------------------
class EVERGREEN_API AppUpdateEvent : public Event
{
public:
	AppUpdateEvent() noexcept {}

	std::string ToString() const noexcept override { return "AppUpdateEvent"; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::AppUpdate; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "AppUpdate"; }
};

// App Render Event -----------------------------------------------------------
class EVERGREEN_API AppRenderEvent : public Event
{
public:
	AppRenderEvent() noexcept {}

	std::string ToString() const noexcept override { return "AppRenderEvent"; }

	// Event Class Category
	virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	static EventType GetStaticType() noexcept { return EventType::AppRender; }
	virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	virtual const char* GetName() const noexcept override { return "AppRender"; }
};

}