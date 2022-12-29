#include <Evergreen.h>

using namespace Evergreen;

class Sandbox : public Evergreen::Application
{
public:
	Sandbox() noexcept
	{
		SetCallbacks();

		m_ui->SetUIRoot("src/json/");
		m_ui->LoadUI("main.json");
	}
	~Sandbox() noexcept override
	{
	}

private:
	void SetCallbacks()
	{
		// Test Button
		JSONLoaders::AddControlFunction("TestButtonOnMouseEnter", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkOrange))));
			}
		);
		JSONLoaders::AddControlFunction("TestButtonOnMouseLeave", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);	
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Orange))));
			}
		);
		JSONLoaders::AddControlFunction("TestButtonOnMouseLButtonDown", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);
				button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::Peru))));
			}
		);
		JSONLoaders::AddControlFunction("TestButtonOnMouseLButtonUp", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);
				// Only need to change the background color if the mouse is still over the button (because if the mouse leaves the button area, the
				// OnMouseLeave event will fire and set the background color anyways)
				if (button->MouseIsOver())
				{
					button->BackgroundBrush(std::move(std::make_unique<SolidColorBrush>(button->GetDeviceResources(), D2D1::ColorF(D2D1::ColorF::DarkOrange))));
				}
			}
		);
		JSONLoaders::AddControlFunction("TestButtonOnClick", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);

			}
		);

		// Test Button 2
		JSONLoaders::AddControlFunction("TestButton2OnMouseEnter", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);

			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnMouseLeave", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);

			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnMouseLButtonDown", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);

			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnMouseLButtonUp", [](Control* control)
			{
				Button* button = static_cast<Button*>(control);

			}
		);
		JSONLoaders::AddControlFunction("TestButton2OnClick", [](Control* control)
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