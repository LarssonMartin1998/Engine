#include "Application.h"

#include "Input.h"
#include "Rendering/Graphics.h"
#include "Fbx/FbxHelper.h"
#include "FileSystem/FileSystem.h"

Application* Application::instance = nullptr;

Application::Application()
: graphics (nullptr)
, input (nullptr)
, fbxHelper (nullptr)
, fileSystem (nullptr)
{

}

Application::Application(const Application& /*application*/)
{

}

Application::~Application()
{

}

LRESULT CALLBACK Application::MessageHandler(HWND hwndArg, UINT umsg, WPARAM wparam , LPARAM lparam)
{
	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwndArg, umsg, wparam, lparam);
		}
	}
}

Application* Application::GetInstance()
{
	assert(instance != nullptr);

	return instance;
}

bool Application::InitializeSingleton()
{
	if (instance)
	{
		return false;
	}

	instance = new Application;
	return true;
}

void Application::ReleaseSingleton()
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}

bool Application::InitializeEngine()
{
	int screenWidth = 0;
	int screenHeight = 0;
	bool result;

	InitializeWindows(screenWidth, screenHeight);

	fbxHelper = new FbxHelper();
	if (!fbxHelper)
	{
		return false;
	}

	result = fbxHelper->Initialize();
	if (!result)
	{
		return false;
	}

	fileSystem = new FileSystem();
	if (!fileSystem)
	{
		return false;
	}

	result = fileSystem->Initialize();
	if (!result)
	{
		return false;
	}

	input = new Input();
	if (!input)
	{
		return false;
	}

	result = input->Initialize();
	if (!result)
	{
		return false;
	}

	graphics = new Graphics();
	if (!graphics)
	{
		return false;
	}

	result = graphics->Initialize(screenWidth, screenHeight, hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void Application::Run()
{
	bool result;
	bool done;
	MSG msg;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

void Application::ShutdownEngine()
{
	if (input)
	{
		input->Shutdown();
		delete input;
		input = nullptr;
	}

	if (graphics)
	{
		graphics->Shutdown();
		delete graphics;
		graphics = nullptr;
	}

	if (fbxHelper)
	{
		fbxHelper->Shutdown();
		delete fbxHelper;
		fbxHelper = nullptr;
	}

	if (fileSystem)
	{
		delete fileSystem;
		fileSystem = nullptr;
	}
}

void Application::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.
	applicationHandle = this;

	// Get the instance of this application.
	hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	applicationName = "Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 1024x768 resolution.
		screenWidth = 1024;
		screenHeight = 768;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, (LPCSTR)applicationName, (LPCSTR)applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void Application::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(hwnd);
	hwnd = NULL;

	// Remove the application instance.
	UnregisterClass((LPCSTR)applicationName, hinstance);
	hinstance = NULL;

	// Release the pointer to this class.
	applicationHandle = NULL;

	return;
}

bool Application::Frame()
{
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if(input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = graphics->Frame();
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return applicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}