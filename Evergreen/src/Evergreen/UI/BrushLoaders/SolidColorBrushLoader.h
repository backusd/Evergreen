#pragma once
#include "pch.h"
#include "BrushLoader.h"
#include "Evergreen/UI/Colors/Brushes.h"
#include "Evergreen/UI/ColorHelper.h"

/*
namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API SolidColorBrushLoader : public BrushLoader
{
public:
	SolidColorBrushLoader(const SolidColorBrushLoader&) = delete;
	void operator=(const SolidColorBrushLoader&) = delete;
	virtual ~SolidColorBrushLoader() noexcept {}

	static std::optional<std::unique_ptr<ColorBrush>> Load(std::shared_ptr<DeviceResources> deviceResources, json& data) noexcept;

private:
	SolidColorBrushLoader() noexcept;

	static SolidColorBrushLoader& Get() noexcept
	{
		static SolidColorBrushLoader loader;
		return loader;
	}

	// bool PreLoadValidation(json& data) noexcept override; <-- Can override this function

	bool ParseColor(SolidColorBrush* brush, json& data) noexcept;

};
#pragma warning( pop )
}
*/