#include <Evergreen.h>
#include <Evergreen/EntryPoint.h>
#include "Evergreen/UI/JSONLoading/ControlLoaders/ButtonLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/SliderFloatLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/SliderIntLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/RadioButtonLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/TextInputLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/ViewportLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/PaneLoader.h"

#include "TestButton.h"

#include "UI/MenuBar/MenuBarButtons.h"

using namespace Evergreen;

#define BUTTON_LOADER(name) JSONLoaders::AddControlLoader(STRINGIFY(name), [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return ButtonLoader::Load<name>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); })


class Editor : public Evergreen::Application
{
public:
	Editor()
	{
		BUTTON_LOADER(FileDropDownButton);
		BUTTON_LOADER(EditDropDownButton);
		BUTTON_LOADER(ViewDropDownButton);
		

		//JSONLoaders::AddControlLoader("TestButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return ButtonLoader::Load<TestButton>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		//JSONLoaders::AddControlLoader("TestSliderFloat", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return SliderFloatLoader::Load<TestSliderFloat>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		//JSONLoaders::AddControlLoader("TestSliderInt", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return SliderIntLoader::Load<TestSliderInt>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		//JSONLoaders::AddControlLoader("TestRadioButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return RadioButtonLoader::Load<TestRadioButton>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		//JSONLoaders::AddControlLoader("TestTextInput", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return TextInputLoader::Load<TestTextInput>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		//JSONLoaders::AddControlLoader("TestViewport", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return ViewportLoader::Load<TestViewport>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		//JSONLoaders::AddControlLoader("TestPane", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return PaneLoader::Load<TestPane>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });



		m_ui->SetUIRoot("src/json/");
		m_ui->LoadUI("main.json");
	}
	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;
	virtual ~Editor() override {}



protected:
	void OnUpdate(const Timer& timer) override
	{

	}
	void OnRender() override
	{

	}

private:

};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Editor();
}