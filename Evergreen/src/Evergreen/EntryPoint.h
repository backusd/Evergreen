#pragma once


#ifdef EG_PLATFORM_WINDOWS

extern Evergreen::Application* Evergreen::CreateApplication();

int main(int argc, char** argv)
{
	Evergreen::Log::Init();

	EG_CORE_ERROR("You made an error");
	EG_CORE_INFO("You got info");
	EG_CORE_TRACE("trace me");

	Evergreen::Application* application = Evergreen::CreateApplication();
	application->Run();
	delete application;
	return 0;
}

#else
#error Evergreen only supports Windows
#endif // EG_PLATFORM_WINDOWS