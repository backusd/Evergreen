#include <Evergreen.h>

class Sandbox : public Evergreen::Application
{
public:
	Sandbox() noexcept
	{

	}
	~Sandbox() noexcept
	{

	}
};


Evergreen::Application* Evergreen::CreateApplication()
{
	return new Sandbox();
}