#include "pch.h"
#include "JSON.h"

/*

#include <sstream>
#include <fstream>

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
JSON::JSON(const JSON& data) noexcept :
	m_json(data.m_json)
{
}
void JSON::operator=(const JSON& data) noexcept
{
	m_json = data.m_json;
}

std::string JSON::ToString() const noexcept
{
	std::ostringstream oss;
	oss << m_json;
	return oss.str();
}

std::vector<std::string> JSON::Keys() const noexcept
{
	std::vector<std::string> keys;
	for (auto& [key, value] : m_json.items())
		keys.push_back(key);
	return keys;
}

std::optional<JSON> JSON::LoadFile(const std::filesystem::path& filePath) noexcept
{
	std::ifstream file;
	file.open(filePath);
	if (file.is_open())
	{
		std::string fileData;
		std::ostringstream oss;
		oss << file.rdbuf();
		fileData = oss.str();
		file.close();

		return JSON(fileData);
	}

	EG_CORE_WARN("{}:{} - JSON::LoadFile failed to open file {}", __FILE__, __LINE__, filePath.string());
	return std::nullopt;
}


}

*/