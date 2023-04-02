#include <Evergreen.h>
#include <Evergreen/EntryPoint.h>
#include "src/MoleculesApp.h"


Evergreen::Application* Evergreen::CreateApplication()
{
	return new MoleculesApp();
}