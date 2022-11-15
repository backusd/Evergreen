#include "pch.h"
#include "JSON.h"

#include <sstream>

namespace Evergreen
{
JSON::JSON(const std::string& data) noexcept
{
	m_json = json::parse(data);
}
JSON::JSON(const json& data) noexcept : 
	m_json(data)
{
}

std::string JSON::ToString() const noexcept
{
	std::ostringstream oss;
	oss << m_json;
	return oss.str();
}

// Template specialization MUST go in the .cpp file
template <>
JSON JSON::Get<JSON>(const std::string& key) const noexcept
{
	return JSON(m_json[key].get<json>());
}


}

