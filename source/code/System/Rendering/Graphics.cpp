 #include "Graphics.h"

#include "D3D.h"
#include "Camera.h"
#include "Model.h"
#include "Bitmap.h"
#include "Light.h"
#include "DiffuseShader.h"
#include "SpecularShader.h"
#include "TextureShader.h"

Graphics::Graphics()
: direct3D (nullptr)
, camera (nullptr)
, model (nullptr)
, bitmap (nullptr)
, light (nullptr)
, specularShader (nullptr)
, textureShader (nullptr)
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
		MessageBox(hwnd, "Could t initalize Direct3D", "Error", MB_OK);
		return false;
	}

	camera = new Camera();
	if (!camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera->SetPosition(0.0f, 0.0f, -5.0f);
	camera->SetRotation(0.0f, 0.0f, 0.0f);

	model = new Model();
	if (!model)
	{
		return false;
	}
	
	//result = model->Initialize(direct3D->GetDevice(), direct3D->GetDeviceContext(), "models/bell.fbx", "textures/stone01.tga");
	result = model->Initialize(direct3D->GetDevice(), direct3D->GetDeviceContext(), "models/cube.txt", "textures/stone01.tga");
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

	bitmap = new Bitmap();
	if (!bitmap)
	{
		return false;
	}

	result = bitmap->Initialize(direct3D->GetDevice(), direct3D->GetDeviceContext(), screenWidth, screenHeight, "textures/stone01.tga", 256, 256);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the bitmap object.", "Error", MB_OK);
		return false;
	}

	light = new Light();
	if (!light)
	{
		return false;
	}

	light->ambientColor.x = 0.125f;
	light->ambientColor.y = 0.125f;
	light->ambientColor.z = 0.125f;
	light->ambientColor.w = 1.0f;

	light->diffuseColor.x = 1.0f;
	light->diffuseColor.y = 1.0f;
	light->diffuseColor.z = 1.0f;
	light->diffuseColor.w = 1.0f;
	
	light->lightDirection.x = -0.5f;
	light->lightDirection.y = 0.1f;
	light->lightDirection.z = 1.0f;

	light->specularColor.x = 1.0f;
	light->specularColor.y = 1.0f;
	light->specularColor.z = 1.0f;
	light->specularColor.w = 1.0f;
	light->specularPower = 32.0f;

	specularShader = new SpecularShader();
	if (!specularShader)
	{
		return false;
	}

	result = specularShader->Initialize(direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the specular shader object.", "Error", MB_OK);
		return false;
	}

	textureShader = new TextureShader();
	if (!textureShader)
	{
		return false;
	}

	result = textureShader->Initialize(direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the texture shader object.", "Error", MB_OK);
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

	if (bitmap)
	{
		bitmap->Shutdown();
		delete bitmap;
		bitmap = nullptr;
	}

	if (light)
	{
		delete light;
		light = nullptr;
	}

	if (specularShader)
	{
		specularShader->Shutdown();
		delete specularShader;
		specularShader = nullptr;
	}

	if (textureShader)
	{
		textureShader->Shutdown();
		delete textureShader;
		textureShader = nullptr;
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
	DirectX::XMMATRIX orthoMatrix;

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix depending on the camera's position.
	camera->Render();

	// Get the world, view, and projection matrices from the camera, and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatri(projectionMatrix);
	direct3D->GetOrthographicMatrix(orthoMatrix);

	direct3D->TurnOffZbuffer();

	result = bitmap->Render(direct3D->GetDeviceContext(), (1024 / 2) - (256 / 2), (768 / 2) - (256 / 2));
	if (!result)
	{
		return false;
	}

	result = textureShader->Render(direct3D->GetDeviceContext(), bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, bitmap->GetTexture());
	if (!result)
	{
		return false;
	}

	direct3D->TurnOnZbuffer();

	//worldMatrix *= DirectX::XMMatrixRotationY(rotation);

	//// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//model->Render(direct3D->GetDeviceContext());

	//result = specularShader->Render(direct3D->GetDeviceContext(), model->GetIndexCount(), camera->GetPosition(), worldMatrix, viewMatrix, projectionMatrix, model->GetTexture(), light->lightDirection, light->diffuseColor, light->ambientColor, light->specularPower, light->specularColor);
	//if (!result)
	//{
	//	return false;
	//}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}