#pragma once
#include "pch.h"
#include "Evergreen/Core.h"

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
	JSON(const std::string&) noexcept;
	JSON(const json&) noexcept;
	JSON(const JSON&) = delete;
	void operator=(const JSON&) = delete;

	inline bool Contains(const std::string& key) const noexcept { return m_json.contains(key); }

	template <typename T>
	T Get(const std::string& key) const noexcept;
//	template <> // template specialization for returning JSON object
//	JSON Get(const std::string& key) const noexcept;

	std::string ToString() const noexcept;

private:
	json m_json;


};
#pragma warning( pop )


template <typename T>
T JSON::Get(const std::string& key) const noexcept 
{ 
	return m_json[key].get<T>(); 
}

//template <>
//JSON JSON::Get(const std::string& key) const noexcept
//{
//	return JSON(m_json[key].get<json>());
//}


}