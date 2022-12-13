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
class EVERGREEN_API GradientBrushLoader : public BrushLoader
{
public:
	GradientBrushLoader(const GradientBrushLoader&) = delete;
	void operator=(const GradientBrushLoader&) = delete;
	virtual ~GradientBrushLoader() noexcept {}

	static std::optional<std::unique_ptr<ColorBrush>> Load(std::shared_ptr<DeviceResources> deviceResources, json& data) noexcept;

private:
	GradientBrushLoader() noexcept;

	static GradientBrushLoader& Get() noexcept
	{
		static GradientBrushLoader loader;
		return loader;
	}

	// bool PreLoadValidation(json& data) noexcept override; <-- Can override this function

	bool ParseStops(GradientBrush* brush, json& data) noexcept;
	bool ParseGamma(GradientBrush* brush, json& data) noexcept;
	bool ParseExtendMode(GradientBrush* brush, json& data) noexcept;
	bool ParseAxis(GradientBrush* brush, json& data) noexcept;

	std::optional<D2D1_COLOR_F> ParseColor(json& data) noexcept;

};
#pragma warning( pop )
}
*/