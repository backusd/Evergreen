#include "pch.h"
#include "BitmapBrush.h"

using Microsoft::WRL::ComPtr;

namespace Evergreen
{
BitmapBrush::BitmapBrush(std::shared_ptr<DeviceResources> deviceResources, const std::wstring& filename,
	TRANSFORM_TO_RECT_METHOD method, const D2D1_BITMAP_BRUSH_PROPERTIES& properties) :
	ColorBrush(deviceResources),
	m_bitmapBrushProperties(properties),
	m_bitmapFileName(filename),
	m_transformMethod(method),
	m_bitmap(nullptr)
{
	// Enforce the brush to be created with a file specified
	EG_CORE_ASSERT(m_bitmapFileName.size() > 0, "File not specified");
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	LoadBitmapFile();
	TransformToRect();
	Refresh();
}
BitmapBrush::BitmapBrush(const BitmapBrush& rhs) :
	ColorBrush(rhs.m_deviceResources),
	m_bitmapBrushProperties(rhs.m_bitmapBrushProperties),
	m_bitmapFileName(rhs.m_bitmapFileName),
	m_transformMethod(rhs.m_transformMethod),
	m_bitmap(nullptr)
{
	LoadBitmapFile();
	TransformToRect();
	Refresh();
}
BitmapBrush& BitmapBrush::operator=(const BitmapBrush& rhs)
{
	ColorBrush::operator=(rhs);

	m_bitmapBrushProperties = rhs.m_bitmapBrushProperties;
	m_bitmapFileName = rhs.m_bitmapFileName;
	m_transformMethod = rhs.m_transformMethod;
	m_bitmap = nullptr;

	LoadBitmapFile();
	TransformToRect();
	Refresh();

	return *this;
}
std::unique_ptr<ColorBrush> BitmapBrush::Duplicate()
{
	return std::move(std::make_unique<BitmapBrush>(*this));
}
void BitmapBrush::LoadBitmapFile(const std::wstring& filename)
{ 
	m_bitmapFileName = filename; 
	LoadBitmapFile();
	TransformToRect();
	Refresh();
}

void BitmapBrush::LoadBitmapFile()
{
	// This method should never be called before a file has been specified
	EG_CORE_ASSERT(m_bitmapFileName.size() > 0, "File not specified");
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	ComPtr<IWICBitmapDecoder> pDecoder = nullptr;
	ComPtr<IWICBitmapFrameDecode> pSource = nullptr;
	ComPtr<IWICFormatConverter> pConverter = nullptr;

	GFX_THROW_INFO(
		m_deviceResources->WICImagingFactory()->CreateDecoderFromFilename(
			m_bitmapFileName.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			pDecoder.ReleaseAndGetAddressOf()
		)
	)

	GFX_THROW_INFO(
		pDecoder->GetFrame(0, pSource.ReleaseAndGetAddressOf())
	)

	GFX_THROW_INFO(
		m_deviceResources->WICImagingFactory()->CreateFormatConverter(&pConverter)
	)

	GFX_THROW_INFO(
		pConverter->Initialize(
			pSource.Get(),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		)
	)

	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateBitmapFromWicBitmap(
			pConverter.Get(),
			NULL,
			m_bitmap.ReleaseAndGetAddressOf()
		)
	)
}

void BitmapBrush::Refresh()
{
	// This method should never be called if we have not loaded the bitmap
	EG_CORE_ASSERT(m_bitmap != nullptr, "bitmap not loaded");
	EG_CORE_ASSERT(m_deviceResources != nullptr, "No device resources");

	ComPtr<ID2D1BitmapBrush> bitmapBrush = nullptr;

	GFX_THROW_INFO(
		m_deviceResources->D2DDeviceContext()->CreateBitmapBrush(
			m_bitmap.Get(),
			m_bitmapBrushProperties,
			m_brushProperties,
			bitmapBrush.ReleaseAndGetAddressOf()
		)
	)

	GFX_THROW_INFO(
		bitmapBrush->QueryInterface<ID2D1Brush>(m_brush.ReleaseAndGetAddressOf())
	)
}

void BitmapBrush::TransformToRect(const D2D1_RECT_F& rect, TRANSFORM_TO_RECT_METHOD method) noexcept
{
	m_drawRegion = rect;
	m_transformMethod = method;

	TransformToRect();
}

void BitmapBrush::TransformToRect() noexcept
{
	// This method should never be called if we have not loaded the bitmap
	EG_CORE_ASSERT(m_bitmap != nullptr, "bitmap not loaded");

	D2D1_POINT_2F topLeft = D2D1::Point2F(m_drawRegion.left, m_drawRegion.top);
	D2D1_SIZE_F scale = D2D1::SizeF(1.0f, 1.0f);

	D2D1_SIZE_F originalBitmapSize = m_bitmap->GetSize();

	EG_CORE_ASSERT(originalBitmapSize.width > 0.0f, "original bitmap width cannot be 0");
	EG_CORE_ASSERT(originalBitmapSize.height > 0.0f, "original bitmap height cannot be 0");

	switch (m_transformMethod)
	{
	case TRANSFORM_TO_RECT_METHOD::SCALE_TO_EXACT_RECT_DIMENSIONS:
		scale.width = (m_drawRegion.right - m_drawRegion.left) / originalBitmapSize.width;
		scale.height = (m_drawRegion.bottom - m_drawRegion.top) / originalBitmapSize.height;
		break;

	case TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_FILL_RECT:
		// Only keep the maximum scale value. This represents the minimum amount of scale that
		// is required for the bitmap to completely fill the given rect
		scale.width = std::max(
			(m_drawRegion.right - m_drawRegion.left) / originalBitmapSize.width,
			(m_drawRegion.bottom - m_drawRegion.top) / originalBitmapSize.height
		);
		scale.height = scale.width;
		break;

	case TRANSFORM_TO_RECT_METHOD::KEEP_XY_RATIO_UNDERFILL_RECT:
		// Only keep the minimum scale value. This represents the scale that
		// is required for the bitmap to fill the axis that requires the most change
		scale.width = std::min(
			(m_drawRegion.right - m_drawRegion.left) / originalBitmapSize.width,
			(m_drawRegion.bottom - m_drawRegion.top) / originalBitmapSize.height
		);
		scale.height = scale.width;
		break;
	}

	// Bitmap brushes are interesting because the way Direct2D works is to set the bitmap to be drawn
	// at the top left of the screen at (0,0). Therefore, it is not enough to just resize the bitmap 
	// and call FillRectangle. The brush itself must also be translated to the top left of the rect area
	// and then scaled down from there.
	SetTransform(
		D2D1::Matrix3x2F::Translation(D2D1::SizeF(topLeft.x, topLeft.y)) *
		D2D1::Matrix3x2F::Scale(scale, topLeft)
	);
}

void BitmapBrush::ExtendMode(D2D1_EXTEND_MODE modeXAndY)
{
	m_bitmapBrushProperties.extendModeX = modeXAndY;
	m_bitmapBrushProperties.extendModeY = modeXAndY;
	Refresh();
}
void BitmapBrush::ExtendMode(D2D1_EXTEND_MODE modeX, D2D1_EXTEND_MODE modeY)
{
	m_bitmapBrushProperties.extendModeX = modeX;
	m_bitmapBrushProperties.extendModeY = modeY;
	Refresh();
}
void BitmapBrush::ExtendModeX(D2D1_EXTEND_MODE mode)
{
	m_bitmapBrushProperties.extendModeX = mode;
	Refresh();
}
void BitmapBrush::ExtendModeY(D2D1_EXTEND_MODE mode)
{
	m_bitmapBrushProperties.extendModeY = mode;
	Refresh();
}
void BitmapBrush::InterpolationMode(D2D1_BITMAP_INTERPOLATION_MODE mode)
{
	m_bitmapBrushProperties.interpolationMode = mode;
	Refresh();
}


}