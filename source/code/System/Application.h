#pragma once

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

class Application;
class Input;
class Graphics;

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Application* applicationHandle = nullptr;

class Application
{
public:

	Application();
	Application(const Application&);
	~Application();

	bool Initialize();
	void Run();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:

	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	bool Frame();

	LPCWSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	Graphics* graphics;
	Input* input;
};

#endif