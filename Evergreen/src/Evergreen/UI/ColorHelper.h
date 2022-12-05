#pragma once
#include "pch.h"


namespace Evergreen
{
	std::optional<D2D1_COLOR_F> ColorFromString(const std::string& colorName) noexcept;
}