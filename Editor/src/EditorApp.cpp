#include <Evergreen.h>
#include <Evergreen/EntryPoint.h>
#include "Evergreen/UI/JSONLoading/ControlLoaders/ButtonLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/SliderFloatLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/SliderIntLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/RadioButtonLoader.h"
#include "Evergreen/UI/JSONLoading/ControlLoaders/TextInputLoader.h"

#include "TestButton.h"

using namespace Evergreen;

class Editor : public Evergreen::Application
{
public:
	Editor()
	{
		JSONLoaders::AddControlLoader("TestButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return ButtonLoader::Load<TestButton>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		JSONLoaders::AddControlLoader("TestSliderFloat", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return SliderFloatLoader::Load<TestSliderFloat>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		JSONLoaders::AddControlLoader("TestSliderInt", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return SliderIntLoader::Load<TestSliderInt>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		JSONLoaders::AddControlLoader("TestRadioButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return RadioButtonLoader::Load<TestRadioButton>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });
		JSONLoaders::AddControlLoader("TestTextInput", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return TextInputLoader::Load<TestTextInput>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });



		m_ui->SetUIRoot("src/json/");
		m_ui->LoadUI("main.json");
	}
	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;
	~Editor() override {}



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