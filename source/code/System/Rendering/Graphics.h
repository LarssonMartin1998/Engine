#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3D;
class Camera;
class Model;
class Bitmap;
class DiffuseShader;
class SpecularShader;
class TextureShader;
class Light;
class Text;

class Graphics
{
public:

	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(int, int, HWND);
	void Shutdown();

	bool Frame();

private:

	bool Render(float);

	D3D* direct3D;
	Camera* camera;
	Model* model;
	Bitmap* bitmap;
	Light* light;
	SpecularShader* specularShader;
	TextureShader* textureShader;
	Text* text;

};

#endif