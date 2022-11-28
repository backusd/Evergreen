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

private:
	void LoadDefaultUI() noexcept;
	void LoadErrorUI() noexcept;
	std::optional<json> LoadJSONFile(std::filesystem::path filePath) noexcept;

	// Load Layout/Control methods
	bool LoadLayoutDetails(Layout* layout, json& data) noexcept;
	bool LoadLayoutRowDefinitions(Layout* layout, json& data) noexcept;
	bool LoadLayoutColumnDefinitions(Layout* layout, json& data) noexcept;
	bool LoadSubLayout(Layout* parent, json& data, const std::string& name) noexcept;
	bool LoadTextControl(Layout* parent, json& data, const std::string& name) noexcept;
	
	bool ParseTextStyle(json& data, std::shared_ptr<TextStyle>& style) noexcept;
	bool ParseGlobalTextStyles(json& data) noexcept;
	bool ParseRowColumnPosition(json& data, RowColumnPosition& position) noexcept;
	bool ParseRowColumnTypeAndSize(json& data, Layout* layout, RowColumnType& type, float& size) noexcept;


	// Function to handle 'import' key
	bool ImportJSON(json& data) noexcept;

	json						m_jsonRoot;
	std::shared_ptr<Window>		m_window;
	std::unique_ptr<Layout>		m_rootLayout;
	std::vector<std::string>	m_errorMessages;
	std::filesystem::path		m_jsonRootDirectory;

	std::shared_ptr<DeviceResources> m_deviceResources;

	std::unordered_map<std::string, std::shared_ptr<TextStyle>> m_textStylesMap;


};
#pragma warning( pop )


}

template <>
struct std::formatter<json> : std::formatter<std::string> {
	auto format(json& _json, std::format_context& ctx) {
		std::ostringstream oss;
		oss << _json;
		return formatter<std::string>::format(oss.str(), ctx);
	}
};