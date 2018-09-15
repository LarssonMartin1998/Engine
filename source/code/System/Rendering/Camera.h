#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <directxmath.h>

class Camera
{
public:

	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float x, float y, float z) { positionX = x; positionY = y; positionZ = z; }
	void SetRotation(float x, float y, float z) { rotationX = x; rotationY = y; rotationZ = z; }

	DirectX::XMFLOAT3 GetPosition() { return DirectX::XMFLOAT3(positionX, positionY, positionZ); }
	DirectX::XMFLOAT3 GetRotation() { return DirectX::XMFLOAT3(rotationX, rotationY, rotationZ); }

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX& viewMatrixReference) { viewMatrixReference = viewMatrix; }

private:

	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;
	DirectX::XMMATRIX viewMatrix;

};

#endif