#pragma once
#include "pch.h"

#include "WindowProperties.h"
#include "Evergreen/Core.h"
#include "Evergreen/Events/Event.h"

namespace Evergreen
{
class EVERGREEN_API Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;

public:
	Window() noexcept = default;
	Window(const Window&) = delete;
	void operator=(const Window&) = delete;	
	virtual ~Window() {}

	virtual void OnUpdate() noexcept = 0;

	virtual unsigned int GetWidth() const noexcept = 0;
	virtual unsigned int GetHeight() const noexcept = 0;

	//virtual void SetEventCallback(const EventCallbackFn& callback) noexcept = 0;
	//virtual void SetVSync(bool enabled) noexcept = 0;
	//virtual bool IsVSync() const noexcept = 0;

	virtual std::optional<int> ProcessMessages() const noexcept = 0;

	static Window* Create(const WindowProperties& props = WindowProperties()) noexcept;
};
















}