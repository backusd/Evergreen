#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Events/KeyEvent.h"
#include "Evergreen/Events/MouseEvent.h"
#include "Evergreen/Rendering/DeviceResources.h"
#include "Layout.h"
#include "Evergreen/Window/Window.h"
#include "Evergreen/UI/Controls/Text.h"
#include "Evergreen/UI/ControlLoaders/ControlLoader.h"
#include "Evergreen/UI/StyleLoaders/TextStyleLoader.h"
#include "Evergreen/UI/GlobalJsonData.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;



namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API UI
{
public:
	UI(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<Window> window) noexcept;
	UI(const UI&) = delete;
	void operator=(const UI&) = delete;

	void SetUIRoot(const std::string& directoryPath) noexcept { m_jsonRootDirectory = std::filesystem::path(directoryPath); }
	void LoadUI(const std::string& fileName) noexcept;
	void Render(DeviceResources* deviceResources) const noexcept;

	void OnWindowResize(WindowResizeEvent& e) noexcept;
	void OnMouseMove(MouseMoveEvent& e) noexcept;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept;



	static void SetControlLoaderFunction(const std::string& controlName, std::function<bool(std::shared_ptr<DeviceResources>, Layout*, json&, const std::string&, GlobalJsonData*)> func) noexcept { m_loadControlFunctions[controlName] = func; }
	static void SetStyleLoaderFunction(const std::string& styleName, std::function<std::optional<std::shared_ptr<Style>>(std::shared_ptr<DeviceResources>, json&, const std::string&)> func) noexcept { m_loadStyleFunctions[styleName] = func; }
	//static void SetBrushLoaderFunction(const std::string& brushName, std::function<std::optional<std::unique_ptr<ColorBrush>>(std::shared_ptr<DeviceResources>, json&)> func) noexcept { m_loadColorBrushFunctions[brushName] = func; }

private:
	void LoadDefaultUI() noexcept;
	void LoadErrorUI() noexcept;
	std::optional<json> LoadJSONFile(std::filesystem::path filePath) noexcept;

	// Load Layout/Control methods
	bool LoadLayoutDetails(Layout* layout, json& data) noexcept;
	bool LoadLayoutRowDefinitions(Layout* layout, json& data) noexcept;
	bool LoadLayoutColumnDefinitions(Layout* layout, json& data) noexcept;
	bool LoadSubLayout(Layout* parent, json& data, const std::string& name) noexcept;
	
	bool ParseGlobalStyles(json& data) noexcept;
	std::optional<RowColumnPosition> ParseRowColumnPosition(json& data) noexcept;
	std::optional<std::tuple<RowColumnType, float>> ParseRowColumnTypeAndSize(json& data, Layout* layout) noexcept;


	// Function to handle 'import' key
	bool ImportJSON(json& data) noexcept;

	json						m_jsonRoot;
	std::shared_ptr<Window>		m_window;
	std::unique_ptr<Layout>		m_rootLayout;
	std::vector<std::string>	m_errorMessages;
	std::filesystem::path		m_jsonRootDirectory;

	std::shared_ptr<DeviceResources> m_deviceResources;

	std::shared_ptr<GlobalJsonData> m_globalJsonData;

	static std::unordered_map<std::string, std::function<bool(std::shared_ptr<DeviceResources>, Layout*, json&, const std::string&, GlobalJsonData*)>> m_loadControlFunctions;
	static std::unordered_map<std::string, std::function<std::optional<std::shared_ptr<Style>>(std::shared_ptr<DeviceResources>, json&, const std::string&)>> m_loadStyleFunctions;
	//static std::unordered_map<std::string, std::function<std::optional<std::unique_ptr<ColorBrush>>(std::shared_ptr<DeviceResources>, json&)>> m_loadColorBrushFunctions;

};
#pragma warning( pop )


}

