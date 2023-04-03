#include "MoleculesApp.h"

using namespace Evergreen;

MoleculesApp::MoleculesApp()
{
	SetCallbacks();
	m_ui->SetUIRoot("src/json/");
	m_ui->LoadUI("main.json");

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

void MoleculesApp::SetCallbacks()
{
	// MenuBar Callbacks
	SetMenuBarCallbacks();
	SetMenuBarFileDropDownCallbacks();
	SetMenuBarEditDropDownCallbacks();
	SetMenuBarViewDropDownCallbacks();
	SetSearchTextInputCallbacks();
	SetViewportCallbacks();

	// Materials
	SetMaterialEditCallbacks();

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
			D2D1_COLOR_F color = filePane->GetVisible() ? m_menuBarButtonColorPaneOpen : m_menuBarButtonColorMouseOverPaneClosed;
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, color)));
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
			}
			else if (button->MouseIsOver())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
			}
		}
	);

	// Edit ----------------------------------------------------------------------------------------------
	JSONLoaders::AddCallback("EditDropDownButtonOnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* editPane = m_ui->GetPane("EditDropDownPane");
			D2D1_COLOR_F color = editPane->GetVisible() ? m_menuBarButtonColorPaneOpen : m_menuBarButtonColorMouseOverPaneClosed;
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, color)));
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

					Pane* filePane = m_ui->GetPane("FileDropDownPane");
					filePane->SetVisible(true);
				}
				else if (viewButton->ContainsPoint(mme.GetX(), mme.GetY()))
				{
					editPane->SetVisible(false);
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));

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
			}
			else if (button->MouseIsOver())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
			}
		}
	);

	// View ----------------------------------------------------------------------------------------------
	JSONLoaders::AddCallback("ViewDropDownButtonOnMouseEnter",
		[this](Button* button, MouseMoveEvent& e)
		{
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
			D2D1_COLOR_F color = viewPane->GetVisible() ? m_menuBarButtonColorPaneOpen : m_menuBarButtonColorMouseOverPaneClosed;
			button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, color)));
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
			}
			else if (button->MouseIsOver())
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorMouseOverPaneClosed)));
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
					}
					else if (pane == viewPane)
					{
						viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
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
					}
					else if (pane == viewPane)
					{
						viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
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
					}
					else if (pane == editPane)
					{
						editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
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
			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);

	// Open
	JSONLoaders::AddCallback("FileDropDown_OpenButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);
			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);

	// Save
	JSONLoaders::AddCallback("FileDropDown_SaveButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);
			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);

	// Save As
	JSONLoaders::AddCallback("FileDropDown_SaveAsButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);
			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);

	// Close
	JSONLoaders::AddCallback("FileDropDown_CloseButton_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* filePane = m_ui->GetPane("FileDropDownPane");
			filePane->SetVisible(false);
			Button* fileButton = m_ui->GetControlByName<Button>("FileDropDownButton");
			fileButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);
}
void MoleculesApp::SetMenuBarEditDropDownCallbacks()
{
	// Edit 1
	JSONLoaders::AddCallback("EditDropDown_Edit1Button_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* editPane = m_ui->GetPane("EditDropDownPane");
			editPane->SetVisible(false);
			Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
			editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);

	// Edit 2
	JSONLoaders::AddCallback("EditDropDown_Edit2Button_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* editPane = m_ui->GetPane("EditDropDownPane");
			editPane->SetVisible(false);
			Button* editButton = m_ui->GetControlByName<Button>("EditDropDownButton");
			editButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
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
			Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
			viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);

	// View 2
	JSONLoaders::AddCallback("ViewDropDown_View2Button_OnClick",
		[this](Button* button, MouseButtonReleasedEvent& e)
		{
			// Close the pane
			Pane* viewPane = m_ui->GetPane("ViewDropDownPane");
			viewPane->SetVisible(false);
			Button* viewButton = m_ui->GetControlByName<Button>("ViewDropDownButton");
			viewButton->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(m_deviceResources, m_menuBarButtonColorDefault)));
		}
	);
}

// Material Callbacks
void MoleculesApp::SetMaterialEditCallbacks()
{
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


