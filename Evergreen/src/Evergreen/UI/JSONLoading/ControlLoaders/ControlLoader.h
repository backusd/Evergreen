#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/UI/Layout.h"
#include "Evergreen/Exceptions/JSONLoadersException.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;


namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API ControlLoader
{
public:
	ControlLoader(const ControlLoader&) = delete;
	void operator=(const ControlLoader&) = delete;
	virtual ~ControlLoader() noexcept {}

protected:
	ControlLoader() noexcept = default;

	RowColumnPosition ParseRowColumnPosition(json& data);
	Margin ParseMargin(json& data);
	unsigned int ParseID(json& data);

	std::string m_name;

	
};
#pragma warning( pop )


}

