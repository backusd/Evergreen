#include <Evergreen.h>
#include "Rendering/Scene.h"
#include "Simulation/Simulation.h"

class MoleculesApp : public Evergreen::Application
{
public:
	MoleculesApp();
	MoleculesApp(const MoleculesApp&) = delete;
	void operator=(const MoleculesApp&) = delete;
	~MoleculesApp() override {}

	Scene* GetScene() noexcept { return m_scene.get(); }


protected:
	std::unique_ptr<Scene> m_scene;
	std::unique_ptr<Simulation> m_simulation;
	Evergreen::Viewport* m_viewport;

	// Controls we want to keep track of 
	Evergreen::Button* m_rightPanelSelectedTabButton;
	Evergreen::Layout* m_rightPanelContentLayout;
	Evergreen::Layout* m_rightPanelTabsLayout;

	void OnUpdate(const Evergreen::Timer& timer) override;
	void OnRender() override;

private:
	void FinalizeUI();
	void SetCallbacks();
	void SetSearchTextInputCallbacks();

	void SetViewportCallbacks();

	// MenuBar Callbacks
	void SetMenuBarCallbacks();
	void SetMenuBarFileDropDownCallbacks();
	void SetMenuBarEditDropDownCallbacks();
	void SetMenuBarViewDropDownCallbacks();

	// Right Panel (Camera/Material) Callbacks
	void SetGeneralRightPanelCallbacks();
	void SetSimulationCallbacks();
	void SetCameraEditCallbacks();
	void SetMaterialEditCallbacks();
	void SetLightingEditCallbacks();

	// Right Panel Layout Callbacks
	void SetRightPanelLayoutCallbacks();

	// Generic right panel methods
	void RightPanelAddTab(Evergreen::Button* button, Evergreen::MouseButtonReleasedEvent& e, const std::string& tabButtonName, const std::string& tabButtonJSON, const std::string& contentJSON);
	void RightPanelCloseTab(Evergreen::MouseButtonReleasedEvent& e, const std::string& tabButtonName);
	void RightPanelTabOnClick(Evergreen::Button* button, const std::string& contentJSON);

	// Generic methods
	void ChangeButtonBackground(Evergreen::Button* button, const D2D1_COLOR_F& color) const;
	void ChangeButtonBackground(Evergreen::Button* button, D2D1::ColorF::Enum color) const;
	void ChangeButtonBackgroundAndTextColor(Evergreen::Button* button, const D2D1_COLOR_F& buttonColor, const D2D1_COLOR_F& textColor) const;
	void ChangeButtonBackgroundAndTextColor(Evergreen::Button* button, D2D1::ColorF::Enum buttonColor, const D2D1_COLOR_F& textColor) const;
	void ChangeButtonBackgroundAndTextColor(Evergreen::Button* button, const D2D1_COLOR_F& buttonColor, D2D1::ColorF::Enum textColor) const;
	void ChangeButtonBackgroundAndTextColor(Evergreen::Button* button, D2D1::ColorF::Enum buttonColor, D2D1::ColorF::Enum textColor) const;

	const D2D1_COLOR_F m_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
	const D2D1_COLOR_F m_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
	const D2D1_COLOR_F m_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

	const D2D1_COLOR_F m_rightPanelTabColorSelected = D2D1::ColorF(0.16f, 0.16f, 0.16f);
	const D2D1_COLOR_F m_rightPanelTabColorNotSelected = D2D1::ColorF(0.2f, 0.2f, 0.2f);
	const D2D1_COLOR_F m_rightPanelTabColorMouseOver = D2D1::ColorF(0.25f, 0.25f, 0.25f);
	const D2D1_COLOR_F m_rightPanelTabColorMouseDown = D2D1::ColorF(0.3f, 0.3f, 0.3f);

};