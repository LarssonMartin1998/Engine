#include <windows.h>

#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application* app = new Application();
	bool result;

	if (!app)
	{
		return 0;
	}

	result = app->Initialize();
	if (result)
	{
		app->Run();
	}

	app->Shutdown();
	delete app;
	app = nullptr;

	return 0;
}