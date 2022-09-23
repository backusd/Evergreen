#include <Evergreen.h>

class Sandbox : public Evergreen::Application
{
public:
	Sandbox()
	{

	}
	~Sandbox()
	{

	}
};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}