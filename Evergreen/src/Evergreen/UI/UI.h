#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Events/ApplicationEvent.h"
#include "Evergreen/Events/KeyEvent.h"
#include "Evergreen/Events/MouseEvent.h"
#include "Evergreen/Rendering/DeviceResources.h"
#include "Layout.h"
#include "Evergreen/Window/Window.h"
#include "JSONLoading/ControlLoaders/ControlLoader.h"
#include "JSONLoading/StyleLoaders/TextStyleLoader.h"
#include "JSONLoading/JSONLoaders.h"
#include "Controls/Button.h"
#include "Controls/RoundedButton.h"
#include "Controls/ScrollableLayout.h"
#include "Controls/TextInput.h"
#include "Controls/Pane.h"
#include "Controls/Slider.h"

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
	
	void Update() noexcept;
	void Render() const noexcept;

	void OnChar(CharEvent& e) noexcept;
	void OnKeyPressed(KeyPressedEvent& e) noexcept;
	void OnKeyReleased(KeyReleasedEvent& e) noexcept;
	void OnWindowResize(WindowResizeEvent& e) noexcept;
	void OnMouseMove(MouseMoveEvent& e) noexcept;
	void OnMouseButtonPressed(MouseButtonPressedEvent& e) noexcept;
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e) noexcept;
	void OnMouseScrolledVertical(MouseScrolledEvent& e) noexcept;
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e) noexcept;
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e) noexcept;


	ND inline Control* GetControlByName(const std::string& name) const noexcept;
	ND inline Control* GetControlByID(unsigned int id) const noexcept;

	inline Pane* AddPane(std::unique_ptr<Pane> pane, const std::string& name) noexcept;
	ND inline Pane* GetPane(const std::string& name) noexcept;
	void RemovePane(Pane* pane) noexcept;
	void RemovePane(const std::string& name) noexcept;
	void BringPaneToForeground(Pane* pane) noexcept;
	void BringPaneToForeground(const std::string& name) noexcept;
	void ClearHandlingControlAndLayout() noexcept;

private:
	void LoadDefaultUI() noexcept;
	void LoadErrorUI() noexcept;
	void RemovePaneFromVector(Pane* pane) noexcept;

	std::shared_ptr<Window>		m_window;
	std::unique_ptr<Layout>		m_rootLayout;

	std::vector<std::unique_ptr<Pane>> m_panes;
	std::unordered_map<std::string, Pane*> m_panesMap;


	std::filesystem::path		m_jsonRootDirectory;

	std::shared_ptr<DeviceResources> m_deviceResources;

	// Controls/Layouts that have handled key/mouse events
	Control* m_mouseHandlingControl;
	Layout*  m_mouseHandlingLayout;

	Control* m_keyboardHandlingControl;
	Layout*  m_keyboardHandlingLayout;
};
#pragma warning( pop )


}

