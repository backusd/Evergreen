#pragma once
#include "pch.h"

#include <unordered_map>


class WindowsMessageMap
{
public:
	WindowsMessageMap() noexcept;
	WindowsMessageMap(const WindowsMessageMap&) = delete;
	void operator=(const WindowsMessageMap&) = delete;

	std::string operator()(DWORD msg, LPARAM lParam, WPARAM wParam) const noexcept;

private:
	std::unordered_map<DWORD, std::string> map;
};