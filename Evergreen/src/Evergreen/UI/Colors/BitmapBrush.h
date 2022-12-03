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
	BitmapBrush(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename) noexcept;
	BitmapBrush(const BitmapBrush&) noexcept;
	void operator=(const BitmapBrush&) noexcept;

	void Refresh() noexcept override;

	void LoadBitmapFile(const std::wstring& filename) noexcept;

	void TransformToRect(const D2D1_RECT_F& rect, TRANSFORM_TO_RECT_METHOD method) noexcept;

private:
	void LoadBitmapFile() noexcept;

	std::wstring							m_bitmapFileName;
	D2D1_BITMAP_BRUSH_PROPERTIES			m_bitmapBrushProperties;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>	m_bitmap;

	float m_top;
	float m_left;
	float m_scaleX;
	float m_scaleY;


};
#pragma warning( pop )
}