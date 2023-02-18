#include <Evergreen.h>

using namespace Evergreen;

class Sandbox : public Evergreen::Application
{
public:
	Sandbox() noexcept
	{
		//SetCallbacks();
		//m_ui->SetUIRoot("src/json/");
		//m_ui->LoadUI("main.json");
	}
	~Sandbox() noexcept override
	{
	}

private:
	void SetCallbacks()
	{
		// MenuBar Callbacks
		SetMenuBarCallbacks();
		SetMenuBarFileDropDownCallbacks();
		SetMenuBarEditDropDownCallbacks();
		SetMenuBarViewDropDownCallbacks();





		// TESTING ================================================================================
		// SliderFloat
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseEnteredCircle", 
			[](SliderFloat* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<Evergreen::SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Red));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseExitedCircle", 
			[](SliderFloat* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Purple));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnBeginDragging", 
			[](SliderFloat* slider, MouseButtonPressedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnStoppedDragging", 
			[](SliderFloat* slider, MouseButtonReleasedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnValueChanged", 
			[](SliderFloat* slider, SliderFloatValueChangedEvent& e)
			{
				EG_TRACE("Slider: {}", slider->GetValue());
			}
		);

		// SliderInt
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseEnteredCircle",
			[](SliderInt* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<Evergreen::SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Red));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnMouseExitedCircle",
			[](SliderInt* slider, MouseMoveEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Purple));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnBeginDragging",
			[](SliderInt* slider, MouseButtonPressedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Green));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnStoppedDragging",
			[](SliderInt* slider, MouseButtonReleasedEvent& e)
			{
				std::unique_ptr<ColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(slider->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
				slider->SetCircleBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("SliderFloatExample_OnValueChanged",
			[](SliderInt* slider, SliderIntValueChangedEvent& e)
			{
				EG_TRACE("Slider: {}", slider->GetValue());
			}
		);

		// Pane
		JSONLoaders::AddCallback("Pane1_OnMouseEnteredTitleBar", 
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue));
				pane->SetTitleBarBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseExitedTitleBar",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));
				pane->SetTitleBarBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseEnteredContentRegion",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::White));
				pane->SetBackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseExitedContentRegion",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::unique_ptr<SolidColorBrush> brush = std::make_unique<Evergreen::SolidColorBrush>(pane->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkGray));
				pane->SetBackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("Pane1_OnMouseMoved",
			[](Pane* pane, MouseMoveEvent& e)
			{
				std::string newTitle = std::format("x: {}, y: {}", e.GetX(), e.GetY());
				pane->ClearTitleBarLayoutAndAddTitle(newTitle);
			}
		);

		// TextInput
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


		// Test Button
		JSONLoaders::AddCallback("TestButtonOnMouseEnter", 
			[](Button* button, MouseMoveEvent& e)
			{
				float c = 0.35f;
				std::vector<D2D1_GRADIENT_STOP> stops{ 
					{0.0f, D2D1::ColorF(c, c, c)}, 
					{0.4f, D2D1::ColorF(D2D1::ColorF::Black)},
					{1.0f, D2D1::ColorF(D2D1::ColorF::Black)}
				};

				std::unique_ptr<RadialBrush> brush = std::make_unique<RadialBrush>(
					button->GetDeviceResources(),
					stops,
					D2D1::Point2F()
				);

				brush->SetDrawRegionRectModifier(
					[](const D2D1_RECT_F& rect, ColorBrush* brush) -> D2D1_RECT_F
					{
						float height = rect.bottom - rect.top;
						float width = rect.right - rect.left;
						float centerX = rect.left + (width / 2.0f);
						float centerY = rect.top + (height / 2.0f);

						float halfLength = std::max(height, width);

						return D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength);
					}
				);

				button->BackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("TestButtonOnMouseLeave", 
			[](Button* button, MouseMoveEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Black))));
			}
		);		
		JSONLoaders::AddCallback("TestButtonOnMouseMoved", 
			[](Button* button, MouseMoveEvent& e)
			{
				D2D1_RECT_F rect = button->AllowedRegion();

				float height = rect.bottom - rect.top;
				float width = rect.right - rect.left;
				float halfLength = std::max(height, width);

				float centerX = e.GetX();
				float centerY = e.GetY();

				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
				backgroundBrush->SetDrawRegion(
					D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength)
				);

//				float centerX = rect.left + ((rect.right - rect.left) / 2.0f);
//				float centerY = rect.top + ((rect.bottom - rect.top) / 2.0f);
//
//				MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
//				
//				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
//				backgroundBrush->GradientOriginOffset(D2D1::Point2F(mme.GetX() - centerX, mme.GetY() - centerY));
//
//				
//				
//				D2D1_RECT_F rect = button->AllowedRegion();
//				float centerX = rect.left + ((rect.right - rect.left) / 2.0f);
//				float centerY = rect.top + ((rect.bottom - rect.top) / 2.0f);
//
//				D2D1_POINT_2F mouseLocation = button->MouseLocation();
//
//				std::unique_ptr<RadialBrush> brush = std::make_unique<RadialBrush>(
//					button->GetDeviceResources(), 
//					D2D1::ColorF(D2D1::ColorF::Gainsboro),
//					D2D1::ColorF(D2D1::ColorF::Black),
//					D2D1::Point2F(mouseLocation.x - centerX, mouseLocation.y - centerY)
//				);
//
//				button->BackgroundBrush(std::move(brush));
//
//				//button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Orange))));
			}
		);
		JSONLoaders::AddCallback("TestButtonOnMouseLButtonDown", 
			[](Button* button, MouseButtonPressedEvent& e)
			{
				// button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Peru))));
			}
		);
		JSONLoaders::AddCallback("TestButtonOnMouseLButtonUp", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
				// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
				// OnMouseLeave event will fire and set the background color anyways)
				
				//if (button->MouseIsOver())
				//{
				//	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkOrange))));
				//}
			}
		);
		JSONLoaders::AddCallback("TestButtonOnClick", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
			}
		);

		// Test Button 2
		JSONLoaders::AddCallback("TestButton2OnMouseEnter", 
			[](Button* button, MouseMoveEvent& e)
			{
				std::vector<D2D1_GRADIENT_STOP> stops{
					{0.0f, D2D1::ColorF(D2D1::ColorF::Pink)},
					{0.2f, D2D1::ColorF(D2D1::ColorF::Blue)},
					{1.0f, D2D1::ColorF(D2D1::ColorF::Blue)}
				};

				std::unique_ptr<RadialBrush> brush = std::make_unique<RadialBrush>(
					button->GetDeviceResources(),
					stops,
					D2D1::Point2F()
					);

				brush->SetDrawRegionRectModifier(
					[](const D2D1_RECT_F& rect, ColorBrush* brush) -> D2D1_RECT_F
					{
						float height = rect.bottom - rect.top;
						float width = rect.right - rect.left;
						float centerX = rect.left + (width / 2.0f);
						float centerY = rect.top + (height / 2.0f);

						float halfLength = std::max(height, width);

						return D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength);
					}
				);

				button->BackgroundBrush(std::move(brush));
			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseLeave", 
			[](Button* button, MouseMoveEvent& e)
			{
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue))));
			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseMoved", 
			[](Button* button, MouseMoveEvent& e)
			{
				D2D1_RECT_F rect = button->AllowedRegion();

				float height = rect.bottom - rect.top;
				float width = rect.right - rect.left;
				float halfLength = std::max(height, width);

				float centerX = e.GetX();
				float centerY = e.GetY();

				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
				backgroundBrush->SetDrawRegion(
					D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength)
				);

			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseLButtonDown", 
			[](Button* button, MouseButtonPressedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("TestButton2OnMouseLButtonUp", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
			}
		);
		JSONLoaders::AddCallback("TestButton2OnClick", 
			[](Button* button, MouseButtonReleasedEvent& e)
			{
			}
		);

	}

	// MenuBar Callbacks
	void SetMenuBarCallbacks()
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
	void SetMenuBarFileDropDownCallbacks()
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
	void SetMenuBarEditDropDownCallbacks()
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
	void SetMenuBarViewDropDownCallbacks()
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

	const D2D1_COLOR_F m_menuBarButtonColorDefault = D2D1::ColorF(0.16f, 0.16f, 0.16f);
	const D2D1_COLOR_F m_menuBarButtonColorMouseOverPaneClosed = D2D1::ColorF(0.35f, 0.35f, 0.35f);
	const D2D1_COLOR_F m_menuBarButtonColorPaneOpen = D2D1::ColorF(0.25f, 0.25f, 0.25f);

};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}