#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"

/*
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API JSON
{
public:
	JSON() noexcept {}
	JSON(const std::string&) noexcept;
	JSON(const json&) noexcept;
	JSON(const JSON&) noexcept;
	void operator=(const JSON&) noexcept;

	JSON operator[](const std::string& key) const noexcept { return JSON(m_json[key]); }

	inline bool Contains(const std::string& key) const noexcept { return m_json.contains(key); }

	template <typename T>
	T Get() const noexcept { return m_json.get<T>(); }

	std::string ToString() const noexcept;

	std::vector<std::string> Keys() const noexcept;

	bool IsArray() const noexcept { return m_json.is_array(); }
	bool IsBinary() const noexcept { return m_json.is_binary(); }
	bool IsBoolean() const noexcept { return m_json.is_boolean(); }
	bool IsDiscarded() const noexcept { return m_json.is_discarded(); }
	bool IsNull() const noexcept { return m_json.is_null(); }
	bool IsNumber() const noexcept { return m_json.is_number(); }
	bool IsNumberFloat() const noexcept { return m_json.is_number_float(); }
	bool IsNumberInteger() const noexcept { return m_json.is_number_integer(); }
	bool IsNumberUnsigned() const noexcept { return m_json.is_number_unsigned(); }
	bool IsObject() const noexcept { return m_json.is_object(); }
	bool IsPrimitive() const noexcept { return m_json.is_primitive(); }
	bool IsString() const noexcept { return m_json.is_string(); }
	bool IsStructured() const noexcept { return m_json.is_structured(); }

	static std::optional<JSON> LoadFile(const std::filesystem::path& filePath) noexcept;

private:
	json m_json;

};
#pragma warning( pop )

}

template <>
struct std::formatter<Evergreen::JSON> : std::formatter<std::string> {
	auto format(Evergreen::JSON& _json, std::format_context& ctx) {
		return formatter<std::string>::format(_json.ToString(), ctx);
	}
};

*/