////////////////////////////////////////////////////////////////////////////////
// Filename: CameraClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CameraClass.h"


CameraClass::CameraClass()
{
	this->m_positionX = 0.0f;
	this->m_positionY = 0.0f;
	this->m_positionZ = 0.0f;

	this->m_rotationX = 0.0f;
	this->m_rotationY = 0.0f;
	this->m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	this->m_positionX = x;
	this->m_positionY = y;
	this->m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	this->m_rotationX = x;
	this->m_rotationY = y;
	this->m_rotationZ = z;
	return;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(this->m_positionX, this->m_positionY, this->m_positionZ);
}

D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(this->m_rotationX, this->m_rotationY, this->m_rotationZ);
}

void CameraClass::Render()
{
	D3DXVECTOR3 up;
	D3DXVECTOR3 position;
	D3DXVECTOR3 lookAt;
	float yaw;
	float pitch;
	float roll;
	D3DXMATRIX rotationMatrix;

	//Setup the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	//Create the rotation matrix from the yaw, pitch and roll values
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	//Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	//Translate the rotated camera position to the location of the viewer
	lookAt += position;

	//Finally create the view matrix from the three updated vectors
	D3DXMatrixLookAtLH(&this->m_viewMatrix, &position, &lookAt, &up);
	return;
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = this->m_viewMatrix;
	return;
}