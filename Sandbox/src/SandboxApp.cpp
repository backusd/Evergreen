#include <Evergreen.h>

class Sandbox : public Evergreen::Application
{
public:
	Sandbox() noexcept
	{
		LoadUI("main.json");
	}
	~Sandbox() noexcept
	{

	}
};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}