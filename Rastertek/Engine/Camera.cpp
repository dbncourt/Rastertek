////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Camera.h"


Camera::Camera()
{
	this->m_position = { 0.0f, 0.0f, 0.0f };
	this->m_rotation = { 0.0f, 0.0f, 0.0f };
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(D3DXVECTOR3 position)
{
	this->m_position = position;
}

void Camera::SetRotation(D3DXVECTOR3 rotation)
{
	this->m_rotation = rotation;
}

D3DXVECTOR3 Camera::GetPosition()
{
	return this->m_position;
}

D3DXVECTOR3 Camera::GetRotation()
{
	return this->m_rotation;
}

void Camera::Render()
{
	D3DXVECTOR3 up;
	D3DXVECTOR3 eye;
	D3DXVECTOR3 lookAt;

	//Setup the vector that points upwards
	up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Setup the position of the camera in the world.
	eye = this->m_position;

	// Calculate the rotation in radians.
	float radians = this->m_rotation.y * 0.0174532925f;

	// Setup where the camera is looking by default.
	lookAt = D3DXVECTOR3(
		(sinf(radians) + this->m_position.x),
		this->m_position.y,
		(cosf(radians) + this->m_position.z)
		);

	// Create the view matrix from the three vectors.
	D3DXMatrixLookAtLH(&this->m_viewMatrix, &eye, &lookAt, &up);
}

void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = this->m_viewMatrix;
}

void Camera::RenderReflection(float height)
{
	D3DXVECTOR3 up;
	D3DXVECTOR3 eye;
	D3DXVECTOR3 lookAt;

	// Setup the vector that points upwards
	up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Setup the position of the camera in the world
	// For planar reflection invert the Y position of the camera.
	eye = D3DXVECTOR3(
		this->m_position.x,
		(-this->m_position.y + (height * 2.0f)),
		this->m_position.z);

	// Calculate the rotation in radians
	float radians = this->m_position.y * 0.0174532925f;

	// Setup where the camera is looking
	lookAt = D3DXVECTOR3(
		(sinf(radians) + this->m_position.x),
		eye.y,
		(cosf(radians) + this->m_position.z));

	// Create the view matrix from the three vectors
	D3DXMatrixLookAtLH(&this->m_reflectionViewMatrix, &eye, &lookAt, &up);
}

void Camera::GetReflectionViewMatrix(D3DXMATRIX& reflectionViewMatrix)
{
	reflectionViewMatrix = this->m_reflectionViewMatrix;
}