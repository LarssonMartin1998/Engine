#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifdef DEBUG
#include <assert.h>
#endif

class Input;
class Graphics;
class FbxHelper;

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Application
{
public:

	static Application* GetInstance();
	static bool InitializeSingleton();
	static void ReleaseSingleton();

	bool InitializeEngine();
	void ShutdownEngine();
	void Run();

	FbxHelper* GetFbxHelper() { return fbxHelper; }

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:

	static Application* instance;

	Application();
	Application(const Application&);
	~Application();

	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	bool Frame();

	LPCSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	FbxHelper* fbxHelper;
	Graphics* graphics;
	Input* input;
};

static Application* applicationHandle = nullptr;

#endif