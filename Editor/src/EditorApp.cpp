#include <Evergreen.h>
#include <Evergreen/EntryPoint.h>

using namespace Evergreen;

class Editor : public Evergreen::Application
{
public:
	Editor()
	{
		//m_ui->SetUIRoot("src/json/");
		//m_ui->LoadUI("main.json");
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