#include "Graphics.h"

#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "ColorShader.h"

Graphics::Graphics()
: direct3D (nullptr)
, camera (nullptr)
, model (nullptr)
, colorShader (nullptr)
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

	camera = new Camera();
	if (!camera)
	{
		return false;
	}

	model = new Model();
	if (!model)
	{
		return false;
	}

	result = model->Initialize(direct3D->GetDevice());
	if (!result)
	{
		return false;
	}

	colorShader = new ColorShader();
	if (!colorShader)
	{
		MessageBox(hwnd, "Could not initialize the color shader object.", "Error", MB_OK);
		return false;
	}

	result = colorShader->Initialize(direct3D->GetDevice(), hwnd);
	if (!result)
	{
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

	if (camera)
	{
		delete camera;
		camera = nullptr;
	}

	if (model)
	{
		model->Shutdown();
		delete model;
		model = nullptr;
	}

	if (colorShader)
	{
		colorShader->Shutdown();
		delete colorShader;
		colorShader = nullptr;
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
	bool result;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix depending on the camera's position.
	camera->Render();

	// Get the world, view, and projection matrices from the camera, and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatri(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	model->Render(direct3D->GetDeviceContext());

	// Render the model using the color shader.
	result = colorShader->Render(direct3D->GetDeviceContext(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}