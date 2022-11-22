#include <Evergreen.h>

class Sandbox : public Evergreen::Application
{
public:
	Sandbox() noexcept
	{
		//m_ui->SetUIRoot("src/json/");
		//m_ui->LoadUI("main.json");
	}
	~Sandbox() noexcept
	{

	}
};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}