#pragma once

extern Evergreen::Application* Evergreen::CreateApplication();

int main(int argc, char** argv)
{
	Evergreen::Log::Init();

	EG_CORE_INFO("Log Initialized");
	EG_CORE_INFO("Launching Application...");

	try
	{
		std::unique_ptr<Evergreen::Application> app = std::unique_ptr<Evergreen::Application>(Evergreen::CreateApplication());
		app->Run();
	}
    catch (const Evergreen::BaseException& e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

	return 0;
}