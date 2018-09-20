#include <windows.h>

#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	bool result;

	Application::InitializeSingleton();
	Application* app = Application::GetInstance();

	if (!app)
	{
		return 0;
	}

	result = app->InitializeEngine();
	if (result)
	{
		app->Run();
	}

	app->ShutdownEngine();
	app->ReleaseSingleton();

	return 0;
}