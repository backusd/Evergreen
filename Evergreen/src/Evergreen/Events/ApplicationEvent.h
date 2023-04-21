#pragma once
#include "pch.h"
#include "Event.h"


namespace Evergreen
{
// Window Resize Event -----------------------------------------------------------
class EVERGREEN_API WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(unsigned int width, unsigned int height) noexcept :
		m_width(width), m_height(height) {}
	WindowResizeEvent(const WindowResizeEvent&) = delete;
	WindowResizeEvent& operator=(const WindowResizeEvent&) = delete;

	ND inline unsigned int GetWidth() const noexcept { return m_width; }
	ND inline unsigned int GetHeight() const noexcept { return m_height; }

	ND std::string ToString() const noexcept override { return std::format("WindowResizeEvent: width = {}, height = {}", m_width, m_height); }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }
	
	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::WindowResize; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "WindowResize"; }

private:
	const unsigned int m_width;
	const unsigned int m_height;
};



// Window Create Event -----------------------------------------------------------
class EVERGREEN_API WindowCreateEvent : public Event
{
public:
	WindowCreateEvent(unsigned int top, unsigned int left, unsigned int width, unsigned int height) noexcept :
		m_top(top), m_left(left), m_width(width), m_height(height)
	{}
	WindowCreateEvent(const WindowCreateEvent&) = delete;
	WindowCreateEvent& operator=(const WindowCreateEvent&) = delete;

	ND std::string ToString() const noexcept override { return std::format("WindowCreateEvent (top: {}, left: {}, width: {}, height: {}", m_top, m_left, m_width, m_height); }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::WindowCreate; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "WindowCreate"; }

private:
	const unsigned int m_top;
	const unsigned int m_left;
	const unsigned int m_width;
	const unsigned int m_height;
};

// Window Close Event -----------------------------------------------------------
class EVERGREEN_API WindowCloseEvent : public Event
{
public:
	WindowCloseEvent() noexcept {}
	WindowCloseEvent(const WindowCloseEvent&) = delete;
	WindowCloseEvent& operator=(const WindowCloseEvent&) = delete;

	ND std::string ToString() const noexcept override { return "WindowCloseEvent"; }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::WindowClose; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "WindowClose"; }
};

// App Tick Event -----------------------------------------------------------
class EVERGREEN_API AppTickEvent : public Event
{
public:
	AppTickEvent() noexcept {}
	AppTickEvent(const AppTickEvent&) = delete;
	AppTickEvent& operator=(const AppTickEvent&) = delete;

	ND std::string ToString() const noexcept override { return "AppTickEvent"; }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::AppTick; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "AppTick"; }
};

// App Update Event -----------------------------------------------------------
class EVERGREEN_API AppUpdateEvent : public Event
{
public:
	AppUpdateEvent() noexcept {}
	AppUpdateEvent(const AppUpdateEvent&) = delete;
	AppUpdateEvent& operator=(const AppUpdateEvent&) = delete;

	ND std::string ToString() const noexcept override { return "AppUpdateEvent"; }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::AppUpdate; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "AppUpdate"; }
};

// App Render Event -----------------------------------------------------------
class EVERGREEN_API AppRenderEvent : public Event
{
public:
	AppRenderEvent() noexcept {}
	AppRenderEvent(const AppRenderEvent&) = delete;
	AppRenderEvent& operator=(const AppRenderEvent&) = delete;

	ND std::string ToString() const noexcept override { return "AppRenderEvent"; }

	// Event Class Category
	ND virtual int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	// Event class type
	ND static EventType GetStaticType() noexcept { return EventType::AppRender; }
	ND virtual EventType GetEventType() const noexcept override { return GetStaticType(); }
	ND virtual const char* GetName() const noexcept override { return "AppRender"; }
};

}

EVENT_FORMATTER(WindowResizeEvent)
EVENT_FORMATTER(WindowCreateEvent)
EVENT_FORMATTER(WindowCloseEvent)
EVENT_FORMATTER(AppTickEvent)
EVENT_FORMATTER(AppUpdateEvent)
EVENT_FORMATTER(AppRenderEvent)
