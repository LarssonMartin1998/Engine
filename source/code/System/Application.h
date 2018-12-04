#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>

class Input;
class Graphics;
class FbxHelper;
class FileSystem;

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
	FileSystem* GetFileSystem() { return fileSystem; }

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

	FileSystem* fileSystem;
	FbxHelper* fbxHelper;
	Graphics* graphics;
	Input* input;
};

static Application* applicationHandle = nullptr;

#endif