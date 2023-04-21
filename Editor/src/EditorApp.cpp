#include <Evergreen.h>
#include <Evergreen/EntryPoint.h>
#include "Evergreen/UI/JSONLoading/ControlLoaders/ButtonLoader.h"

#include "TestButton.h"

using namespace Evergreen;

class Editor : public Evergreen::Application
{
public:
	Editor()
	{
		JSONLoaders::AddControlLoader("TestButton", [](std::shared_ptr<DeviceResources> deviceResources, Layout* parentLayout, json& data, const std::string& controlName, std::optional<RowColumnPosition> rowColumnPositionOverride) -> Control* { return ButtonLoader::Load<TestButton>(deviceResources, parentLayout, data, controlName, rowColumnPositionOverride); });



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