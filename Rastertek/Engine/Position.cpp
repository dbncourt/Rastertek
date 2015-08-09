#include "Position.h"


Position::Position()
{
	this->m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	this->m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	this->m_frameTime = 0;
	this->m_leftSpeed = 0;
	this->m_rightSpeed = 0;
}

Position::Position(const Position& other)
{
}


Position::~Position()
{
}

void Position::SetPosition(_In_ D3DXVECTOR3 position)
{
	this->m_position = position;
}

void Position::GetPosition(_Out_ D3DXVECTOR3& position)
{
	position = this->m_position;
}

void Position::SetRotation(_In_ D3DXVECTOR3 rotation)
{
	this->m_rotation = rotation;
}

void Position::GetRotation(_Out_ D3DXVECTOR3 rotation)
{
	rotation = this->m_rotation;
}

void Position::SetFrameTime(float time)
{
	this->m_frameTime = time;
}

void Position::MoveLeft(bool keydown)
{
	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown)
	{
		this->m_leftSpeed += this->m_frameTime * 0.001f;

		if (this->m_leftSpeed > (this->m_frameTime * 0.03f))
		{
			this->m_leftSpeed = this->m_frameTime * 0.03f;
		}
	}
	else
	{
		this->m_leftSpeed -= this->m_frameTime * 0.0007f;
		if (this->m_leftSpeed < 0.0f)
		{
			this->m_leftSpeed = 0.0f;
		}
	}

	//Update the rotation using the turning speed
	float radians = this->m_rotation.y * 0.0174532925f;

	// Update the position
	this->m_position.x -= cosf(radians) * this->m_leftSpeed;
	this->m_position.z -= sinf(radians) * this->m_leftSpeed;
}

void Position::MoveRight(bool keydown)
{
	//If the key is pressed increase the speed at which the camera turns right. Else slow down the turn speed
	if (keydown)
	{
		this->m_rightSpeed += this->m_frameTime * 0.001f;

		if (this->m_rightSpeed > (this->m_frameTime * 0.03f))
		{
			this->m_rightSpeed = this->m_frameTime * 0.03f;
		}
	}
	else
	{
		this->m_rightSpeed -= this->m_frameTime * 0.0007f;
		
		if (this->m_rightSpeed < 0.0f)
		{
			this->m_rightSpeed = 0.0f;
		}
	}

	//Update the rotation using the turning speed
	float radians = this->m_rotation.y * 0.0174532925f;

	// Update the position
	this->m_position.x += cosf(radians) * this->m_rightSpeed;
	this->m_position.z += sinf(radians) * this->m_rightSpeed;
}