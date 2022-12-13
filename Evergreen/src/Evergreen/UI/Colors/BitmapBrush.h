#pragma once
#include "pch.h"
#include "ColorBrush.h"


namespace Evergreen
{
enum class TRANSFORM_TO_RECT_METHOD
{
	SCALE_TO_EXACT_RECT_DIMENSIONS,
	KEEP_XY_RATIO_FILL_RECT,
	KEEP_XY_RATIO_UNDERFILL_RECT
};

// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API BitmapBrush : public ColorBrush
{
public:
	// Add parameters after DeviceResources must have a default so that BrushLoader can create the brush
	BitmapBrush(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) noexcept;
	BitmapBrush(const BitmapBrush&) noexcept;
	void operator=(const BitmapBrush&) noexcept;

	void Refresh() noexcept override;

	void LoadBitmapFile(const std::wstring& filename) noexcept;
	void TransformToRect(const D2D1_RECT_F& rect, TRANSFORM_TO_RECT_METHOD method = TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_FILL_RECT) noexcept;

	void ExtendMode(D2D1_EXTEND_MODE modeXAndY) noexcept;
	void ExtendMode(D2D1_EXTEND_MODE modeX, D2D1_EXTEND_MODE modeY) noexcept;
	void ExtendModeX(D2D1_EXTEND_MODE mode) noexcept;
	void ExtendModeY(D2D1_EXTEND_MODE mode) noexcept;
	void InterpolationMode(D2D1_BITMAP_INTERPOLATION_MODE mode) noexcept;

	const std::wstring& BitmapFileName() const noexcept { return m_bitmapFileName; }
	const D2D1_BITMAP_BRUSH_PROPERTIES& BitMapBrushProperties() const noexcept { return m_bitmapBrushProperties; }
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> BitMap() const noexcept { return m_bitmap; }

private:
	void LoadBitmapFile() noexcept;

	std::wstring							m_bitmapFileName;
	D2D1_BITMAP_BRUSH_PROPERTIES			m_bitmapBrushProperties;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>	m_bitmap;


};
#pragma warning( pop )
}