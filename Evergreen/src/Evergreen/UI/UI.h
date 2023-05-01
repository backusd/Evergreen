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
#include "Controls.h"
#include "Evergreen/Utils/Timer.h"

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
	UI& operator=(const UI&) = delete;

	void SetUIRoot(const std::string& directoryPath) noexcept { m_jsonRootDirectory = std::filesystem::path(directoryPath); }
	void LoadUI(const std::string& fileName) noexcept;

	void LoadControlsFromFile(const std::string& fileName, Layout* parentLayout, std::optional<RowColumnPosition> rowColumnPositionOverride = std::nullopt);
	void LoadLayoutFromFile(const std::string& fileName, Layout* layoutToFill);

	void Update(const Timer& timer);
	void Render() const;

	void OnChar(CharEvent& e);
	void OnKeyPressed(KeyPressedEvent& e);
	void OnKeyReleased(KeyReleasedEvent& e);
	void OnWindowResize(WindowResizeEvent& e);
	void OnMouseMove(MouseMoveEvent& e);
	void OnMouseButtonPressed(MouseButtonPressedEvent& e);
	void OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	void OnMouseScrolledVertical(MouseScrolledEvent& e);
	void OnMouseScrolledHorizontal(MouseScrolledEvent& e);
	void OnMouseButtonDoubleClick(MouseButtonDoubleClickEvent& e);

	template <class T>
	ND inline T* GetControlByName(const std::string& name) const noexcept;
	template <class T>
	ND inline T* GetControlByID(unsigned int id) const noexcept;

	ND Layout* GetLayoutByName(const std::string& name) noexcept;

	template<typename T>
	inline Pane* AddPane(std::unique_ptr<T> derivedPane, const std::string& name) noexcept requires (std::is_base_of_v<Pane, T>);
	ND inline Pane* GetPane(const std::string& name) noexcept;
	ND inline bool PaneExists(const std::string& name) const noexcept { return m_panesMap.find(name) != m_panesMap.end(); }
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

	// Keep track of whether or not the mouse is actively over a Pane
	bool m_mouseIsOverAPane;

	// Controls/Layouts that have handled key/mouse events
	Control* m_mouseHandlingControl;
	Layout*  m_mouseHandlingLayout;

	Control* m_keyboardHandlingControl;
	Layout*  m_keyboardHandlingLayout;

	// Keep track of which mouse buttons are in use
	bool m_mouseLButtonDown;
	bool m_mouseMButtonDown;
	bool m_mouseRButtonDown;
	bool m_mouseX1ButtonDown;
	bool m_mouseX2ButtonDown;
};
#pragma warning( pop )

template <class T>
T* UI::GetControlByName(const std::string& name) const noexcept
{
	Control* c = nullptr;
	for (const auto& pane : m_panes)
	{
		c = pane->GetControlByName(name);
		if (c != nullptr)
			return static_cast<T*>(c);
	}

	return m_rootLayout->GetControlByName<T>(name);
}
template <class T>
T* UI::GetControlByID(unsigned int id) const noexcept
{
	Control* c = nullptr;
	for (const auto& pane : m_panes)
	{
		c = pane->GetControlByID(id);
		if (c != nullptr)
			return static_cast<T*>(c);
	}

	return m_rootLayout->GetControlByID<T>(id);
}

template<typename T>
Pane* UI::AddPane(std::unique_ptr<T> derivedPane, const std::string& name) noexcept requires (std::is_base_of_v<Pane, T>)
{
	EG_CORE_ASSERT(m_panesMap.find(name) == m_panesMap.end(), std::format("Pane with name '{}' already exists", name));
	EG_CORE_ASSERT(name.size() > 0, "Pane name cannot be empty");
	EG_CORE_ASSERT(derivedPane != nullptr, "Input pane should not be nullptr");

	// NOTE: Must cast the derived Pane class to Pane* which can then be stored in the vector
	m_panes.push_back(std::move(std::unique_ptr<Pane>(static_cast<Pane*>(derivedPane.release()))));
	m_panesMap[name] = m_panes.back().get();
	return m_panes.back().get();
}

}

