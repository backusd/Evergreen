#pragma once
#include "pch.h"
#include "Layout.h"



namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Pane
{
public:
	Pane(std::shared_ptr<DeviceResources> deviceResources,
		UI* ui,
		float top,
		float left,
		float height,
		float width,
		bool resizeable = true,
		bool relocatable = true,
		std::unique_ptr<ColorBrush> backgroundBrush = nullptr,
		std::unique_ptr<ColorBrush> borderBrush = nullptr,
		float borderWidth = 0.0f,
		bool headerBar = true,
		std::unique_ptr<ColorBrush> headerBarBrush = nullptr,
		const std::string& title = ""
	);
	Pane(const Pane&) = delete;
	void operator=(const Pane&) = delete;
	virtual ~Pane() noexcept {}

	void Update() noexcept;
	void Render() const noexcept;

	inline Row* AddRow(RowColumnDefinition definition);
	inline Column* AddColumn(RowColumnDefinition definition);

private:
	void InitializeLayoutWithHeaderBar();
	void InitializeLayoutWithoutHeaderBar();

	std::shared_ptr<DeviceResources> m_deviceResources;
	UI* m_ui;
	std::unique_ptr<Layout> m_layout;
	Layout* m_contentLayout;
	float m_top;
	float m_left;
	float m_height;
	float m_width;
	bool m_resizeable;
	bool m_relocatable;
	std::unique_ptr<ColorBrush> m_backgroundBrush;
	std::unique_ptr<ColorBrush> m_borderBrush;
	float m_borderWidth;
	bool m_headerBar;
	std::unique_ptr<ColorBrush> m_headerBarBrush;
	std::string m_title;
	bool m_minimized;
	bool m_visible;
};
#pragma warning( pop )

}