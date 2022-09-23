#pragma once


#ifdef EG_PLATFORM_WINDOWS

extern Evergreen::Application* Evergreen::CreateApplication();

int main(int argc, char** argv)
{
	Evergreen::Application* application = Evergreen::CreateApplication();
	application->Run();
	delete application;
	return 0;
}

#else
#error Evergreen only supports Windows
#endif // EG_PLATFORM_WINDOWS