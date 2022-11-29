#pragma once
#include "pch.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

template <>
struct std::formatter<json> : std::formatter<std::string> {
	auto format(json& _json, std::format_context& ctx) {
		std::ostringstream oss;
		oss << _json;
		return formatter<std::string>::format(oss.str(), ctx);
	}
};