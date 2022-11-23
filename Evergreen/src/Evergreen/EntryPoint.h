#pragma once

extern Evergreen::Application* Evergreen::CreateApplication();

int main(int argc, char** argv)
{
	Evergreen::Log::Init();

	EG_CORE_INFO("Log Initialized");
	EG_CORE_INFO("Launching Application...");

	std::unique_ptr<Evergreen::Application> app = std::unique_ptr<Evergreen::Application>(Evergreen::CreateApplication());
	app->Run();
	return 0;
}