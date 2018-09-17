#include "Graphics.h"

#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"""
#include "DiffuseShader.h"

Graphics::Graphics()
: direct3D (nullptr)
, camera (nullptr)
, model (nullptr)
, light (nullptr)
, diffuseShader (nullptr)
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

	// Set the initial position of the camera.
	camera->SetPosition(0.0f, 2.5f, -5.0f);
	camera->SetRotation(30.0f, 0.0f, 0.0f);

	model = new Model();
	if (!model)
	{
		return false;
	}

	result = model->Initialize(direct3D->GetDevice(), direct3D->GetDeviceContext(), "data/models/cube.txt", "data/textures/stone01.tga");
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

	light = new Light();
	if (!light)
	{
		return false;
	}

	light->diffuseColor.x = 1.0f;
	light->diffuseColor.y = 1.0f;
	light->diffuseColor.z = 1.0f;
	light->diffuseColor.w = 1.0f;
	
	light->lightDirection.x = 0.0f;
	light->lightDirection.y = -1.0f;
	light->lightDirection.z = 1.0f;

	diffuseShader = new DiffuseShader();
	if (!diffuseShader)
	{
		return false;
	}

	result = diffuseShader->Initialize(direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the diffuse shader object.", "Error", MB_OK);
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

	if (light)
	{
		delete light;
		light = nullptr;
	}

	if (diffuseShader)
	{
		diffuseShader->Shutdown();
		delete diffuseShader;
		diffuseShader = nullptr;
	}
}

bool Graphics::Frame()
{
	bool result;
	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += 3.14f * 0.001f;

	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render(float rotation)
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

	worldMatrix *= DirectX::XMMatrixRotationY(rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	model->Render(direct3D->GetDeviceContext());

	// Render the model using the diffuse shader.
	result = diffuseShader->Render(direct3D->GetDeviceContext(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, model->GetTexture(), light->lightDirection, light->diffuseColor);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}