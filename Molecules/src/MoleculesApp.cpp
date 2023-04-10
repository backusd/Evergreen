#include "MoleculesApp.h"

using namespace Evergreen;

MoleculesApp::MoleculesApp() :
	m_rightPanelSelectedTabButton(nullptr),
	m_rightPanelContentLayout(nullptr)
{
	SetCallbacks();
	m_ui->SetUIRoot("src/json/");
	m_ui->LoadUI("main.json");

	// Always start with the "Simulation" button as selected
	m_rightPanelSelectedTabButton = m_ui->GetControlByName<Button>("RightPanel_SimulationButton");
	EG_ASSERT(m_rightPanelSelectedTabButton != nullptr, "Could not find Button");

	// Cache Pointers to parts of the layout we will frequently need
	m_rightPanelContentLayout = m_ui->GetLayoutByName("RightPanelLayout_ContentLayout");
	EG_ASSERT(m_rightPanelContentLayout != nullptr, "Could not find Layout");
	m_rightPanelTabsLayout = m_ui->GetLayoutByName("RightPanelLayout_TopButtonsLayout");
	EG_ASSERT(m_rightPanelTabsLayout != nullptr, "Could not find Layout");

	// There may be some callbacks that we wish to invoke immediately, but might require a lookup of 
	// 1 or more Layouts or Controls. Therefore, they cannot safely be invoked until after the entire
	// UI has been loaded.
	FinalizeUI();

	// Initialize the Simulation
	m_simulation = std::make_unique<Simulation>();
	//m_simulation->Add(Element::Hydrogen, { 0.0f, 0.0f, 0.0f },  {  0.0f,  0.0f,  0.0f });
	//m_simulation->Add(Element::Helium, { 0.0f, 0.0f, 0.0f },    { -0.1f,  0.9f,  0.0f });
	//m_simulation->Add(Element::Lithium, { 0.0f, 0.0f, 0.0f },   {  0.2f, -0.8f,  0.0f });
	//m_simulation->Add(Element::Beryllium, { 0.0f, 0.0f, 0.0f }, { -0.3f, -0.7f,  0.0f });
	//m_simulation->Add(Element::Boron, { 0.0f, 0.0f, 0.0f },     {  0.4f,  0.6f,  0.0f });
	//m_simulation->Add(Element::Carbon, { 0.0f, 0.0f, 0.0f },    { -0.5f,  0.5f,  0.0f });
	//m_simulation->Add(Element::Nitrogen, { 0.0f, 0.0f, 0.0f },  {  0.6f, -0.4f,  0.0f });
	//m_simulation->Add(Element::Oxygen, { 0.0f, 0.0f, 0.0f },    { -0.7f, -0.3f,  0.0f });
	//m_simulation->Add(Element::Flourine, { 0.0f, 0.0f, 0.0f },  {  0.8f,  0.2f,  0.0f });
	//m_simulation->Add(Element::Neon, { 0.0f, 0.0f, 0.0f },      { -0.9f,  0.1f,  0.0f });

	for (unsigned int iii = 0; iii < 20; ++iii)
		m_simulation->Add(Element::Hydrogen, { 0.0f, 0.0f, 0.0f }, { iii / 57.0f,  iii / 48.0f,  iii / 55.0f });

	for (unsigned int iii = 0; iii < 20; ++iii)
		m_simulation->Add(Element::Helium, { 0.0f, 0.0f, 0.0f }, { iii / 57.0f,  iii / -48.0f,  iii / 55.0f });

	for (unsigned int iii = 0; iii < 20; ++iii)
		m_simulation->Add(Element::Lithium, { 0.0f, 0.0f, 0.0f }, { iii / -57.0f,  iii / -48.0f,  iii / -55.0f });


	// Get pointer to the viewport
	m_viewport = m_ui->GetControlByName<Viewport>("MainViewport");

	// Create the scene
	m_scene = std::make_unique<Scene>(m_deviceResources, m_simulation.get(), m_viewport);

	// Set viewport callbacks
	Scene* scene = m_scene.get();
	m_viewport->SetOnCharCallback([scene](Viewport* vp, CharEvent& e) { scene->OnChar(e); });
	m_viewport->SetOnKeyPressedCallback([scene](Viewport* vp, KeyPressedEvent& e) { scene->OnKeyPressed(e); });
	m_viewport->SetOnKeyReleasedCallback([scene](Viewport* vp, KeyReleasedEvent& e) { scene->OnKeyReleased(e); });
	m_viewport->SetOnMouseEnteredCallback([scene](Viewport* vp, MouseMoveEvent& e) { scene->OnMouseEntered(e); });
	m_viewport->SetOnMouseExitedCallback([scene](Viewport* vp, MouseMoveEvent& e) { scene->OnMouseExited(e); });
	m_viewport->SetOnMouseMovedCallback([scene](Viewport* vp, MouseMoveEvent& e) { scene->OnMouseMoved(e); });
	m_viewport->SetOnMouseScrolledVerticalCallback([scene](Viewport* vp, MouseScrolledEvent& e) { scene->OnMouseScrolledVertical(e); });
	m_viewport->SetOnMouseScrolledHorizontalCallback([scene](Viewport* vp, MouseScrolledEvent& e) { scene->OnMouseScrolledHorizontal(e); });
	m_viewport->SetOnMouseButtonPressedCallback([scene](Viewport* vp, MouseButtonPressedEvent& e) { scene->OnMouseButtonPressed(e); });
	m_viewport->SetOnMouseButtonReleasedCallback([scene](Viewport* vp, MouseButtonReleasedEvent& e) { scene->OnMouseButtonReleased(e); });
	m_viewport->SetOnClickCallback([scene](Viewport* vp, MouseButtonReleasedEvent& e) { scene->OnClick(e); });
	m_viewport->SetOnDoubleClickCallback([scene](Viewport* vp, MouseButtonDoubleClickEvent& e) { scene->OnDoubleClick(e); });

	// Start the simulation
	m_simulation->Play();
}

void MoleculesApp::OnUpdate(const Timer& timer)
{
	m_simulation->Update(timer);
	m_scene->Update(timer);
}
void MoleculesApp::OnRender()
{
	auto context = m_deviceResources->D3DDeviceContext();
	auto device = m_deviceResources->D3DDevice();

	// bind render target
	ID3D11RenderTargetView* const targets[1] = { m_deviceResources->BackBufferRenderTargetView() };
	GFX_THROW_INFO_ONLY(context->OMSetRenderTargets(1u, targets, m_deviceResources->DepthStencilView()));

	float c = 92.0f / 256;
	float background[4] = { c, 0.0f, c, 1.0f };
	GFX_THROW_INFO_ONLY(context->ClearRenderTargetView(m_deviceResources->BackBufferRenderTargetView(), background));
	GFX_THROW_INFO_ONLY(context->ClearDepthStencilView(m_deviceResources->DepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0));

	// configure viewport
	auto vp = m_ui->GetControlByName<Viewport>("MainViewport");
	GFX_THROW_INFO_ONLY(context->RSSetViewports(1, &vp->GetViewport()));

	m_scene->Render();
}

void MoleculesApp::FinalizeUI()
{
	// Right Panel Content Layout --------------------------------------------------------------------
	// Add the content for the Simulation tab
	m_ui->LoadLayoutFromFile("right_panel_simulation_content.json", m_rightPanelContentLayout);
	// Trigger the OnResizeCallback so that the border will be updated before the first Render pass
	m_rightPanelContentLayout->TriggerOnResizeCallback();

}

void MoleculesApp::SetCallbacks()
{
	// MenuBar Callbacks
	SetMenuBarCallbacks();
	SetMenuBarFileDropDownCallbacks();
	SetMenuBarEditDropDownCallbacks();
	SetMenuBarViewDropDownCallbacks();
	SetSearchTextInputCallbacks();
	SetViewportCallbacks();

	// Camera / Materials
	SetGeneralRightPanelCallbacks();
	SetSimulationCallbacks();
	SetCameraEditCallbacks();
	SetMaterialEditCallbacks();

	// Right Panel Layout Callbacks
	SetRightPanelLayoutCallbacks();
}

void MoleculesApp::SetSearchTextInputCallbacks()
{
	JSONLoaders::AddCallback("TextInput1_OnMouseEntered",
		[](TextInput* textInput, MouseMoveEvent& e)
		{
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);
	JSONLoaders::AddCallback("TextInput1_OnMouseExited",
		[](TextInput* textInput, MouseMoveEvent& e)
		{
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Gray));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);
	JSONLoaders::AddCallback("TextInput1_OnMouseMoved",
		[](TextInput* textInput, MouseMoveEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("TextInput1_OnMouseLButtonDown",
		[](TextInput* textInput, MouseButtonPressedEvent& e)
		{
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Coral));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);
	JSONLoaders::AddCallback("TextInput1_OnMouseLButtonUp",
		[](TextInput* textInput, MouseButtonReleasedEvent& e)
		{
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);
	JSONLoaders::AddCallback("TextInput1_OnClick",
		[](TextInput* textInput, MouseButtonReleasedEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("TextInput1_OnEnterKey",
		[](TextInput* textInput, CharEvent& e)
		{
			std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue));
			textInput->SetBackgroundBrush(std::move(bkgdBrush));
		}
	);
	JSONLoaders::AddCallback("TextInput1_OnInputTextChanged",
		[](TextInput* textInput, CharEvent& e)
		{
		}
	);
}

void MoleculesApp::SetViewportCallbacks()
{
	JSONLoaders::AddCallback("Viewport_OnChar",
		[](Viewport* vp, CharEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnKeyPressed",
		[](Viewport* vp, KeyPressedEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnKeyReleased",
		[](Viewport* vp, KeyReleasedEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnMouseEntered",
		[](Viewport* vp, MouseMoveEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnMouseExited",
		[](Viewport* vp, MouseMoveEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnMouseMoved",
		[](Viewport* vp, MouseMoveEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnMouseScrolledVertical",
		[](Viewport* vp, MouseScrolledEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnMouseScrolledHorizontal",
		[](Viewport* vp, MouseScrolledEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnMouseButtonPressed",
		[](Viewport* vp, MouseButtonPressedEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnMouseButtonReleased",
		[](Viewport* vp, MouseButtonReleasedEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnClick",
		[](Viewport* vp, MouseButtonReleasedEvent& e)
		{
		}
	);
	JSONLoaders::AddCallback("Viewport_OnDoubleClick",
		[](Viewport* vp, MouseButtonDoubleClickEvent& e)
		{
		}
	);
}

// MenuBar Callbacks
void MoleculesApp::SetMenuBarCallbacks()
{
	// File ----------------------------------------------------------------------------------------------
	JSONLoaders::AddCallback("FileDropDownButtonOnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			if (filePane->GetVisible())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
			}
			else
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				button->BorderWidth(0.0f);
			}
		}
	);
	JSONLoaders::AddCallback("FileDropDownButtonOnMouseLeave",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* filePane = m_ui->GetPane("FileDropDownPane");

			// First if the file pane is visible, check if the mouse is now over the Edit button
			if (filePane->GetVisible())
			{
				Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
				if (editButton->ContainsPoint(e.GetX(), e.GetY()))
				{
					filePane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					button->BorderWidth(0.0f);

					Pane* editPane = m_ui->GetPane("EditDropDownPane");
					editPane->SetVisible(true);
				}
				else if (filePane->ContainsPoint(e.GetX(), e.GetY()))
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				}
				else
				{
					// Mouse is not over the button or pane, so close the pane
					filePane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					button->BorderWidth(0.0f);
				}
			}
			else
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		}
	);
	JSONLoaders::AddCallback("FileDropDownButtonOnMouseLButtonDown",
		[this](Button* button, MouseButtonPressedEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
		}
	);
	JSONLoaders::AddCallback("FileDropDownButtonOnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SwitchVisible();

			if (filePane->GetVisible())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
			}
			else if (button->MouseIsOver())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				button->BorderWidth(0.0f);
			}
		}
	);

	// Edit ----------------------------------------------------------------------------------------------
	JSONLoaders::AddCallback("EditDropDownButtonOnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* editPane = m_ui->GetPane("EditDropDownPane");
			if (editPane->GetVisible())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
			}
			else
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				button->BorderWidth(0.0f);
			}
		}
	);
	JSONLoaders::AddCallback("EditDropDownButtonOnMouseLeave",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* editPane = m_ui->GetPane("EditDropDownPane");

			// First if the edit pane is visible, check if the mouse is now over the File button or View Button
			if (editPane->GetVisible())
			{
				Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
				Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
				MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
				if (fileButton->ContainsPoint(mme.GetX(), mme.GetY()))
				{
					editPane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					button->BorderWidth(0.0f);

					Pane* filePane = m_ui->GetPane("FileDropDownPane");
					filePane->SetVisible(true);
				}
				else if (viewButton->ContainsPoint(mme.GetX(), mme.GetY()))
				{
					editPane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					button->BorderWidth(0.0f);

					Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
					viewPane->SetVisible(true);
				}
				else if (editPane->ContainsPoint(mme.GetX(), mme.GetY()))
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				}
				else
				{
					// Mouse is not over the button or pane, so close the pane
					editPane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					button->BorderWidth(0.0f);
				}
			}
			else
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		}
	);
	JSONLoaders::AddCallback("EditDropDownButtonOnMouseLButtonDown",
		[this](Button* button, MouseButtonPressedEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
		}
	);
	JSONLoaders::AddCallback("EditDropDownButtonOnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			Pane* editPane = m_ui->GetPane("EditDropDownPane");
			editPane->SwitchVisible();

			if (editPane->GetVisible())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
			}
			else if (button->MouseIsOver())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				button->BorderWidth(0.0f);
			}
		}
	);

	// View ----------------------------------------------------------------------------------------------
	JSONLoaders::AddCallback("ViewDropDownButtonOnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
			if (viewPane->GetVisible())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
			}
			else
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				button->BorderWidth(0.0f);
			}
		}
	);
	JSONLoaders::AddCallback("ViewDropDownButtonOnMouseLeave",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");

			// First if the view pane is visible, check if the mouse is now over the Edit button
			if (viewPane->GetVisible())
			{
				Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
				MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
				if (editButton->ContainsPoint(mme.GetX(), mme.GetY()))
				{
					viewPane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					button->BorderWidth(0.0f);

					Pane* editPane = m_ui->GetPane("EditDropDownPane");
					editPane->SetVisible(true);
				}
				else if (viewPane->ContainsPoint(mme.GetX(), mme.GetY()))
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				}
				else
				{
					// Mouse is not over the button or pane, so close the pane
					viewPane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
					button->BorderWidth(0.0f);
				}
			}
			else
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		}
	);
	JSONLoaders::AddCallback("ViewDropDownButtonOnMouseLButtonDown",
		[this](Button* button, MouseButtonPressedEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
		}
	);
	JSONLoaders::AddCallback("ViewDropDownButtonOnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
			viewPane->SwitchVisible();

			if (viewPane->GetVisible())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
				button->BorderWidth({ 1.0f, 1.0f, 1.0f, 0.0f });
			}
			else if (button->MouseIsOver())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
				button->BorderWidth(0.0f);
			}
		}
	);

	// Pane ----------------------------------------------------------------------------------------------
	JSONLoaders::AddCallback("MenuBarDropDownPaneOnMouseExitedContentRegion",
		[this](Pane* pane, MouseMoveEvent& e)
		{
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			Pane* editPane = m_ui->GetPane("EditDropDownPane");
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
			Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");

			if (fileButton->ContainsPoint(e.GetX(), e.GetY()))
			{
				if (filePane != pane)
				{
					pane->SetVisible(false);
					filePane->SetVisible(true);

					if (pane == editPane)
					{
						editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						editButton->BorderWidth(0.0f);
					}
					else if (pane == viewPane)
					{
						viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						editButton->BorderWidth(0.0f);
					}
				}
			}
			else if (editButton->ContainsPoint(e.GetX(), e.GetY()))
			{
				if (editPane != pane)
				{
					pane->SetVisible(false);
					editPane->SetVisible(true);

					if (pane == filePane)
					{
						fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						fileButton->BorderWidth(0.0f);
					}
					else if (pane == viewPane)
					{
						viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						viewButton->BorderWidth(0.0f);
					}
				}
			}
			else if (viewButton->ContainsPoint(e.GetX(), e.GetY()))
			{
				if (viewPane != pane)
				{
					pane->SetVisible(false);
					viewPane->SetVisible(true);

					if (pane == filePane)
					{
						fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						fileButton->BorderWidth(0.0f);
					}
					else if (pane == editPane)
					{
						editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
						editButton->BorderWidth(0.0f);
					}
				}
			}
			else
			{
				// Mouse is not over pane or menu bar button, so collapse it
				pane->SetVisible(false);
				if (pane == filePane)
					fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
				else if (pane == editPane)
					editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
				else if (pane == viewPane)
					viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			}
		}
	);
}
void MoleculesApp::SetMenuBarFileDropDownCallbacks()
{
	// EACH Item
	JSONLoaders::AddCallback("FileDropDown_Item_OnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
		}
	);
	JSONLoaders::AddCallback("FileDropDown_Item_OnMouseExited",
		[this](Button* button, MouseMoveEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));
		}
	);

	// New
	JSONLoaders::AddCallback("FileDropDown_NewButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			fileButton->BorderWidth(0.0f);
		}
	);

	// Open
	JSONLoaders::AddCallback("FileDropDown_OpenButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			fileButton->BorderWidth(0.0f);
		}
	);

	// Save
	JSONLoaders::AddCallback("FileDropDown_SaveButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			fileButton->BorderWidth(0.0f);
		}
	);

	// Save As
	JSONLoaders::AddCallback("FileDropDown_SaveAsButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			fileButton->BorderWidth(0.0f);
		}
	);

	// Close
	JSONLoaders::AddCallback("FileDropDown_CloseButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			fileButton->BorderWidth(0.0f);
		}
	);
}
void MoleculesApp::SetMenuBarEditDropDownCallbacks()
{
	// Camera -------------------------------------------------------
	JSONLoaders::AddCallback("EditDropDown_CameraButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			this->RightPanelAddTab(button, e, "RightPanel_CameraButton", "right_panel_camera_tab.json", "right_panel_camera_content.json");
		}
	);

	// Materials -----------------------------------------------------
	JSONLoaders::AddCallback("EditDropDown_MaterialsButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			this->RightPanelAddTab(button, e, "RightPanel_MaterialsButton", "right_panel_materials_tab.json", "right_panel_materials_content.json");
		}
	);

	// Lighting -----------------------------------------------------
	JSONLoaders::AddCallback("EditDropDown_LightingButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* editPane = m_ui->GetPane("EditDropDownPane");
			editPane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			// Reset the drop down button back to its original state
			Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
			editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			editButton->BorderWidth(0.0f);
		}
	);
}
void MoleculesApp::SetMenuBarViewDropDownCallbacks()
{
	// View 1
	JSONLoaders::AddCallback("ViewDropDown_View1Button_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
			viewPane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
			viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			viewButton->BorderWidth(0.0f);
		}
	);

	// View 2
	JSONLoaders::AddCallback("ViewDropDown_View2Button_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
			viewPane->SetVisible(false);

			// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
			// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
			// To do this, we inform the event that it should ignore the handling control
			e.IgnoreHandlingControl(true);

			Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
			viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
			viewButton->BorderWidth(0.0f);
		}
	);
}

// General Right Panel Callbacks
void MoleculesApp::SetGeneralRightPanelCallbacks()
{
	JSONLoaders::AddCallback("RightPanelTabOnMouseEnter", 
		[this](Button* button, MouseMoveEvent& e) 
		{
			if (button != m_rightPanelSelectedTabButton) 
			{
				this->ChangeButtonBackgroundAndTextColor(button, m_menuBarButtonColorMouseOverPaneClosed, D2D1::ColorF::White);
			}
		}
	);
	JSONLoaders::AddCallback("RightPanelTabOnMouseLeave",
		[this](Button* button, MouseMoveEvent& e)
		{
			if (button != m_rightPanelSelectedTabButton)
			{
				this->ChangeButtonBackgroundAndTextColor(button, D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f), D2D1::ColorF::LightGray);
			}
		}
	);
	JSONLoaders::AddCallback("RightPanelTabOnMouseLButtonDown",
		[this](Button* button, MouseButtonPressedEvent& e)
		{
			if (button != m_rightPanelSelectedTabButton)
			{
				//this->ChangeButtonBackgroundAndTextColor(button, m_menuBarButtonColorPaneOpen, D2D1::ColorF::White);
				this->ChangeButtonBackground(button, m_menuBarButtonColorPaneOpen);
			}
		}
	);
}

// Simulation Callbacks
void MoleculesApp::SetSimulationCallbacks()
{
	JSONLoaders::AddCallback("SimulationTabOnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			if (button != m_rightPanelSelectedTabButton)
			{
				this->RightPanelTabOnClick(button, "right_panel_simulation_content.json");
			}
		}
	);
}

// Material Callbacks
void MoleculesApp::SetMaterialEditCallbacks()
{
	// Tab Callbacks ---------------------------------------------------------------------------
	// 
	// Materials - Close Button
	JSONLoaders::AddCallback("RightPanel_MaterialsButton_CloseButton_OnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::White)))); 
			Text* text = static_cast<Text*>(button->GetLayout()->GetControl(0)); 
			text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black)))); 
		}
	);
	JSONLoaders::AddCallback("RightPanel_MaterialsButton_CloseButton_OnMouseLeave",
		[this](Button* button, MouseMoveEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)))); 
			Text* text = static_cast<Text*>(button->GetLayout()->GetControl(0)); 
			text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::White))));
		}
	);
	JSONLoaders::AddCallback("RightPanel_MaterialsButton_CloseButton_OnMouseLButtonDown",
		[this](Button* button, MouseButtonPressedEvent& e)
		{
			Text* text = static_cast<Text*>(button->GetLayout()->GetControl(0));
			text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray))));
		}
	);
	JSONLoaders::AddCallback("RightPanel_MaterialsButton_CloseButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			this->RightPanelCloseTab(e, "RightPanel_MaterialsButton");
		}
	);

	// Content Callbacks ---------------------------------------------------------------------------
	JSONLoaders::AddCallback("MaterialDiffuseAlbedoX_OnValueChanged",
		[this](SliderFloat* slider, SliderFloatValueChangedEvent& e)
		{
			Scene* scene = this->GetScene();
			MaterialsArray* materials = scene->GetMaterials();
			materials->materials[0].DiffuseAlbedo.x = e.GetValue();
			scene->UpdateMaterials();
		}
	);
	JSONLoaders::AddCallback("MaterialDiffuseAlbedoY_OnValueChanged",
		[this](SliderFloat* slider, SliderFloatValueChangedEvent& e)
		{
			Scene* scene = this->GetScene();
			MaterialsArray* materials = scene->GetMaterials();
			materials->materials[0].DiffuseAlbedo.y = e.GetValue();
			scene->UpdateMaterials();
		}
	);
	JSONLoaders::AddCallback("MaterialDiffuseAlbedoZ_OnValueChanged",
		[this](SliderFloat* slider, SliderFloatValueChangedEvent& e)
		{
			Scene* scene = this->GetScene();
			MaterialsArray* materials = scene->GetMaterials();
			materials->materials[0].DiffuseAlbedo.z = e.GetValue();
			scene->UpdateMaterials();
		}
	);
}

// Camera Callbacks
void MoleculesApp::SetCameraEditCallbacks()
{
	// Tab Callbacks ---------------------------------------------------------------------------
	// 
	// Materials - Close Button
	JSONLoaders::AddCallback("RightPanel_CameraButton_CloseButton_OnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::White))));
			Text* text = static_cast<Text*>(button->GetLayout()->GetControl(0));
			text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Black))));
		}
	);
	JSONLoaders::AddCallback("RightPanel_CameraButton_CloseButton_OnMouseLeave",
		[this](Button* button, MouseMoveEvent& e)
		{
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f))));
			Text* text = static_cast<Text*>(button->GetLayout()->GetControl(0));
			text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::White))));
		}
	);
	JSONLoaders::AddCallback("RightPanel_CameraButton_CloseButton_OnMouseLButtonDown",
		[this](Button* button, MouseButtonPressedEvent& e)
		{
			Text* text = static_cast<Text*>(button->GetLayout()->GetControl(0));
			text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(D2D1::ColorF::Gray))));
		}
	);
	JSONLoaders::AddCallback("RightPanel_CameraButton_CloseButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			this->RightPanelCloseTab(e, "RightPanel_CameraButton");
		}
	);

	// Content Callbacks ---------------------------------------------------------------------------
}


void MoleculesApp::SetRightPanelLayoutCallbacks()
{
	JSONLoaders::AddLayoutCallback("RightPanelLayout_OnResizeCallback",
		[this](Layout* layout)
		{
			EG_ASSERT(m_rightPanelSelectedTabButton != nullptr, "The right pane selected tab button should never be nullptr.");

			const D2D1_RECT_F& buttonRect = m_rightPanelSelectedTabButton->BackgroundRect();
			layout->BorderTopLeftOffsetX(buttonRect.left - layout->Left());
			layout->BorderTopRightOffsetX(layout->Right() - buttonRect.right);
		}
	);
}

void MoleculesApp::RightPanelAddTab(Button* button, MouseButtonReleasedEvent& e, const std::string& tabButtonName, const std::string& tabButtonJSON, const std::string& contentJSON)
{
	// Close the pane
	Pane* editPane = m_ui->GetPane("EditDropDownPane"); 
	EG_ASSERT(editPane != nullptr, "Pane does not exist"); 
	editPane->SetVisible(false); 

	// When the Pane becomes no longer visible, we must invalidate the UI's mouse captured variables
	// otherwise, it is possible to still click on controls within the Pane after it loses visibility.
	// To do this, we inform the event that it should ignore the handling control
	e.IgnoreHandlingControl(true); 

	// Reset the drop down button back to its original state
	Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton"); 
	EG_ASSERT(editButton != nullptr, "Button does not exist"); 
	editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault))); 
	editButton->BorderWidth(0.0f); 

	// Reset the color of the button back to its non-hovered state
	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorPaneOpen)));

	// Determine if the tab already exists. If not, create it			
	Button* tabButton = m_rightPanelTabsLayout->GetControlByName<Button>(tabButtonName);
	if (tabButton == nullptr)
	{
		// We are going to force the "Simulation" tab to not be removable. When we add a second tab, it will
		// be placed in the second column, which already exists. However, when we add a third, fourth, etc tab,
		// we have to add an additional column to the layout
		if (m_rightPanelTabsLayout->NumberOfControls() > 1) 
			m_rightPanelTabsLayout->AddColumn({ RowColumnType::STAR, 1.0f }); 

		RowColumnPosition rowCol = { 0, static_cast<unsigned int>(m_rightPanelTabsLayout->Columns().size()) - 1, 1, 1 }; 
		m_ui->LoadControlsFromFile(tabButtonJSON, m_rightPanelTabsLayout, rowCol);

		tabButton = m_rightPanelTabsLayout->GetControlByName<Button>(tabButtonName);
		EG_ASSERT(tabButton != nullptr, std::format("Failed to create/find {} button", tabButtonName)); 
	}

	// If the tabButton is already selected, there is nothing else we need to do
	if (m_rightPanelSelectedTabButton == tabButton)
		return;

	// BEFORE updating the m_rightPaneSelectedTabButton pointer, update the current Button
	ChangeButtonBackgroundAndTextColor(m_rightPanelSelectedTabButton, m_rightPanelTabColorNotSelected, D2D1::ColorF::LightGray);
	Button* closeButton = static_cast<Button*>(m_rightPanelSelectedTabButton->GetLayout()->GetFirstControlOfType(Control::ControlType::Button));
	if (closeButton != nullptr)
		ChangeButtonBackgroundAndTextColor(closeButton, m_rightPanelTabColorNotSelected, D2D1::ColorF::LightGray);

	// Set the materials Button as the selected button
	m_rightPanelSelectedTabButton = tabButton;

	// Update the content layouts border along the top edge
	const D2D1_RECT_F& buttonRect = tabButton->BackgroundRect();
	m_rightPanelContentLayout->BorderTopLeftOffsetX(buttonRect.left - m_rightPanelContentLayout->Left()); 
	m_rightPanelContentLayout->BorderTopRightOffsetX(m_rightPanelContentLayout->Right() - buttonRect.right); 

	// Load the material editing controls in the right pane
	m_ui->LoadLayoutFromFile(contentJSON, m_rightPanelContentLayout);
} 
void MoleculesApp::RightPanelCloseTab(MouseButtonReleasedEvent& e, const std::string& tabButtonName)
{
	// First, remove the Button control then delete the column it was in
	//        "Simulation" tab is always at index 0, so we can skip that one
	//
	// NOTE: We MUST ensure the lifetime of the Button for the duration of this lambda. Therefore,
	//       layout->RemoveButton() will return a unique_ptr to the Button which we can store as a
	//       local variable. Once this lambda goes goes out of scope, the local unique_ptr will
	//       officially delete the Button
	std::unique_ptr<Control> _button = nullptr;  
	for (unsigned int iii = 1; iii < m_rightPanelTabsLayout->NumberOfControls(); ++iii)
	{
		// Find the correct Button to remove then break from the loop
		if (m_rightPanelTabsLayout->GetControl(iii)->Name().compare(tabButtonName) == 0)
		{
			_button = m_rightPanelTabsLayout->RemoveControl(iii); 

			// Only remove the Materials column if there are other non-Simulation tabs
			if (m_rightPanelTabsLayout->NumberOfControls() > 1) 
				m_rightPanelTabsLayout->RemoveColumn(iii); 

			break;
		}
	}

	// Make sure the UI doesn't try to send events to the buttont that is being removed
	e.IgnoreHandlingControl(true); 

	// If the button we are removing was not selected, then we don't need to load new contents
	if (static_cast<Button*>(_button.get()) != m_rightPanelSelectedTabButton) 
	{
		// Trigger the OnResize so the layout border gets updated
		m_rightPanelContentLayout->TriggerOnResizeCallback();
		return;
	}

	// Update the color of the Simulation tab text and background 
	m_rightPanelSelectedTabButton = m_ui->GetControlByName<Button>("RightPanel_SimulationButton"); 
	EG_ASSERT(m_rightPanelSelectedTabButton != nullptr, "Could not find Button");
	ChangeButtonBackgroundAndTextColor(m_rightPanelSelectedTabButton, m_rightPanelTabColorSelected, D2D1::ColorF::White);

	// Just default to loading the Simulation content
	m_ui->LoadLayoutFromFile("right_panel_simulation_content.json", m_rightPanelContentLayout);

	// Trigger the OnResize so the layout border gets updated
	m_rightPanelContentLayout->TriggerOnResizeCallback();
}
void MoleculesApp::RightPanelTabOnClick(Button* button, const std::string& contentJSON)
{
	// For the currently selected button, update the background and text color
	ChangeButtonBackgroundAndTextColor(m_rightPanelSelectedTabButton, m_rightPanelTabColorNotSelected, D2D1::ColorF::LightGray);
	Button* closeButton = static_cast<Button*>(m_rightPanelSelectedTabButton->GetLayout()->GetFirstControlOfType(Control::ControlType::Button));
	if (closeButton != nullptr)
		ChangeButtonBackgroundAndTextColor(closeButton, m_rightPanelTabColorNotSelected, D2D1::ColorF::LightGray);

	// Track the new button
	m_rightPanelSelectedTabButton = button;

	// Update the selected button color
	ChangeButtonBackgroundAndTextColor(button, m_rightPanelTabColorSelected, D2D1::ColorF::White);
	closeButton = static_cast<Button*>(button->GetLayout()->GetFirstControlOfType(Control::ControlType::Button));
	if (closeButton != nullptr)
		ChangeButtonBackgroundAndTextColor(closeButton, m_rightPanelTabColorSelected, D2D1::ColorF::White);

	// Load the new right panel contents
	m_ui->LoadLayoutFromFile(contentJSON, m_rightPanelContentLayout);

	// Trigger the OnResize so the layout border gets updated
	m_rightPanelContentLayout->TriggerOnResizeCallback();
}


void MoleculesApp::ChangeButtonBackground(Button* button, const D2D1_COLOR_F& color) const
{
	EG_ASSERT(button != nullptr, "Button cannot be nullptr");
	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, color)));
}
void MoleculesApp::ChangeButtonBackground(Button* button, D2D1::ColorF::Enum color) const 
{
	EG_ASSERT(button != nullptr, "Button cannot be nullptr");
	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(color)))); 
}
void MoleculesApp::ChangeButtonBackgroundAndTextColor(Button* button, const D2D1_COLOR_F& buttonColor, const D2D1_COLOR_F& textColor) const
{
	EG_ASSERT(button != nullptr, "Button cannot be nullptr");
	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, buttonColor)));
	
	Text* text = static_cast<Text*>(button->GetLayout()->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, textColor)));
}
void MoleculesApp::ChangeButtonBackgroundAndTextColor(Button* button, D2D1::ColorF::Enum buttonColor, const D2D1_COLOR_F& textColor) const
{
	EG_ASSERT(button != nullptr, "Button cannot be nullptr");
	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(buttonColor))));

	Text* text = static_cast<Text*>(button->GetLayout()->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, textColor)));
}
void MoleculesApp::ChangeButtonBackgroundAndTextColor(Button* button, const D2D1_COLOR_F& buttonColor, D2D1::ColorF::Enum textColor) const
{
	EG_ASSERT(button != nullptr, "Button cannot be nullptr");
	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, buttonColor)));

	Text* text = static_cast<Text*>(button->GetLayout()->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(textColor))));
}
void MoleculesApp::ChangeButtonBackgroundAndTextColor(Button* button, D2D1::ColorF::Enum buttonColor, D2D1::ColorF::Enum textColor) const
{
	EG_ASSERT(button != nullptr, "Button cannot be nullptr");
	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(buttonColor))));

	Text* text = static_cast<Text*>(button->GetLayout()->GetFirstControlOfType(Control::ControlType::Text));
	EG_ASSERT(text != nullptr, "Should not be calling this function if the button has no Text child control");
	text->SetColorBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, D2D1::ColorF(textColor))));
}