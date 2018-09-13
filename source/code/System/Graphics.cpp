#include "Graphics.h"

#include "D3D.h"

Graphics::Graphics()
: direct3D (nullptr)
{

}

Graphics::Graphics(const Graphics& /*graphics*/)
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	direct3D = new D3D();
	if (!direct3D)
	{
		return false;
	}

	result = direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, "Could not initalize Direct3D", "Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	if (direct3D)
	{
		direct3D->Shutdown();
		delete direct3D;
		direct3D = nullptr;
	}
}

bool Graphics::Frame()
{
	bool result;

	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render()
{
	// Clear the buffers to begin the scene.
	direct3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}