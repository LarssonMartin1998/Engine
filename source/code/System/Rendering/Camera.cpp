#include "Camera.h"

Camera::Camera()
: positionX (0.0f)
, positionY (0.0f)
, positionZ(-10.0f)

, rotationX(0.0f)
, rotationY(0.0f)
, rotationZ(0.0f)
{

}

Camera::Camera(const Camera& camera)
{

}

Camera::~Camera()
{

}

void Camera::Render()
{
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 at;
	DirectX::XMVECTOR upVec;
	DirectX::XMVECTOR positionVec;
	DirectX::XMVECTOR atVec;
	float yaw;
	float pitch;
	float roll;
	DirectX::XMMATRIX rotationMatrix;
	float aDegreeInRadians = 0.0174532925f;

	// Set up the vector that points upwards;
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVec = XMLoadFloat3(&up);

	// Set up the position of the camera in the world.
	position.x = positionX;
	position.y = positionY;
	position.z = positionZ;

	// Load it into a XMVECTOR structure
	positionVec = XMLoadFloat3(&position);

	// Set up where the camera is looking by default.
	at.x = 0.0f;
	at.y = 0.0f;
	at.z = 1.0f;

	// Load it into a XMVECTOR structure.
	atVec = XMLoadFloat3(&at);

	// Set the pitch(X), yaw(Y), and roll (Z) rotatons in radians.
	pitch = rotationX * aDegreeInRadians;
	yaw = rotationY * aDegreeInRadians;
	roll = rotationZ * aDegreeInRadians;

	// Create the rotation matrix from the pitch, yaw and roll.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the at and up vector by the rotation matrix so the view is correctly rotated at the origin.
	atVec = XMVector3TransformCoord(atVec, rotationMatrix);
	upVec = XMVector3TransformCoord(upVec, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	atVec = DirectX::XMVectorAdd(positionVec, atVec);

	// Finally create the veiw matrix from the three updated vectors.
	viewMatrix = DirectX::XMMatrixLookAtLH(positionVec, atVec, upVec);
}

