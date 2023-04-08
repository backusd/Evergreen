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

	// Camera/Material Callbacks
	void SetCameraEditCallbacks();
	void SetMaterialEditCallbacks();

	// Right Panel Layout Callbacks
	void SetRightPanelLayoutCallbacks();

	const D2D1_COLOR_F m_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
	const D2D1_COLOR_F m_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
	const D2D1_COLOR_F m_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

};