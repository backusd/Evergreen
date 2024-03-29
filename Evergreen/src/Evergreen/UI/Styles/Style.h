#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"
#include "Evergreen/Rendering/DeviceResources.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Style
{
public:
	Style(std::shared_ptr<DeviceResources> deviceResources, const std::string& name = "") noexcept;
	Style(const Style& style) noexcept;
	Style& operator=(const Style& rhs) noexcept;
	virtual ~Style() noexcept {}

	ND inline std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_deviceResources; }
	
	void Name(const std::string& name) noexcept { m_name = name; }
	ND inline const std::string& Name() const noexcept { return m_name; }

	virtual std::unique_ptr<Style> Duplicate() const = 0;

protected:
	std::shared_ptr<DeviceResources> m_deviceResources;
	std::string						 m_name;
};
#pragma warning ( pop )
}