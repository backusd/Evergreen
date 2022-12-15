#include <Evergreen.h>

class Sandbox : public Evergreen::Application
{
public:
	Sandbox() noexcept
	{
		m_ui->SetUIRoot("src/json/");
		m_ui->LoadUI("main.json");
	}
	~Sandbox() noexcept
	{
		int iii = 0;
	}
};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}