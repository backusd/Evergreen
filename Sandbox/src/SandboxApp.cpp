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
		// TextInput
		JSONLoaders::AddControlFunction("TextInput1_OnMouseEntered", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddControlFunction("TextInput1_OnMouseExited", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Gray));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddControlFunction("TextInput1_OnMouseMoved", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);
			}
		);
		JSONLoaders::AddControlFunction("TextInput1_OnMouseLButtonDown", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Coral));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddControlFunction("TextInput1_OnMouseLButtonUp", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::LightCoral));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddControlFunction("TextInput1_OnClick", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);

			}
		);
		JSONLoaders::AddControlFunction("TextInput1_OnEnterKey", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);
				std::unique_ptr<SolidColorBrush> bkgdBrush = std::make_unique<SolidColorBrush>(textInput->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue));
				textInput->SetBackgroundBrush(std::move(bkgdBrush));
			}
		);
		JSONLoaders::AddControlFunction("TextInput1_OnInputTextChanged", [](Control* control, Event& e)
			{
				TextInput* textInput = static_cast<TextInput*>(control);

			}
		);







		// Test Button
		JSONLoaders::AddControlFunction("TestButtonOnMouseEnter", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);
				// button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkOrange))));
			
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
		JSONLoaders::AddControlFunction("TestButtonOnMouseLeave", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);	
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Black))));
			}
		);		
		JSONLoaders::AddControlFunction("TestButtonOnMouseMoved", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);
				D2D1_RECT_F rect = button->AllowedRegion();

				float height = rect.bottom - rect.top;
				float width = rect.right - rect.left;
				float halfLength = std::max(height, width);

				MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);

				float centerX = mme.GetX();
				float centerY = mme.GetY();

				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
				backgroundBrush->SetDrawRegion(
					D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength)
				);


				/*


				float centerX = rect.left + ((rect.right - rect.left) / 2.0f);
				float centerY = rect.top + ((rect.bottom - rect.top) / 2.0f);

				MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);
				
				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
				backgroundBrush->GradientOriginOffset(D2D1::Point2F(mme.GetX() - centerX, mme.GetY() - centerY));

				
				
				D2D1_RECT_F rect = button->AllowedRegion();
				float centerX = rect.left + ((rect.right - rect.left) / 2.0f);
				float centerY = rect.top + ((rect.bottom - rect.top) / 2.0f);

				D2D1_POINT_2F mouseLocation = button->MouseLocation();

				std::unique_ptr<RadialBrush> brush = std::make_unique<RadialBrush>(
					button->GetDeviceResources(), 
					D2D1::ColorF(D2D1::ColorF::Gainsboro),
					D2D1::ColorF(D2D1::ColorF::Black),
					D2D1::Point2F(mouseLocation.x - centerX, mouseLocation.y - centerY)
				);

				button->BackgroundBrush(std::move(brush));
				*/
				// button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Orange))));
			}
		);
		JSONLoaders::AddControlFunction("TestButtonOnMouseLButtonDown", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);
				// button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Peru))));
			}
		);
		JSONLoaders::AddControlFunction("TestButtonOnMouseLButtonUp", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);
				// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
				// OnMouseLeave event will fire and set the background color anyways)
				
				//if (button->MouseIsOver())
				//{
				//	button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkOrange))));
				//}
			}
		);
		JSONLoaders::AddControlFunction("TestButtonOnClick", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);

			}
		);

		// Test Button 2
		JSONLoaders::AddControlFunction("TestButton2OnMouseEnter", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);

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
		JSONLoaders::AddControlFunction("TestButton2OnMouseLeave", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Blue))));
			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnMouseMoved", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);
				D2D1_RECT_F rect = button->AllowedRegion();

				float height = rect.bottom - rect.top;
				float width = rect.right - rect.left;
				float halfLength = std::max(height, width);

				MouseMoveEvent& mme = dynamic_cast<MouseMoveEvent&>(e);

				float centerX = mme.GetX();
				float centerY = mme.GetY();

				RadialBrush* backgroundBrush = static_cast<RadialBrush*>(button->BackgroundBrush());
				backgroundBrush->SetDrawRegion(
					D2D1::RectF(centerX - halfLength, centerY - halfLength, centerX + halfLength, centerY + halfLength)
				);

			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnMouseLButtonDown", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);

			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnMouseLButtonUp", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);

			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnClick", [](Control* control, Event& e)
			{
				Button* button = static_cast<Button*>(control);

			}
		);
	}
};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}