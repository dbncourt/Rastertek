#include "Position.h"


Position::Position()
{
	this->m_frameTime = 0;
	this->m_rotationY = 0;
	this->m_leftTurnSpeed = 0;
	this->m_rightTurnSpeed = 0;
}

Position::Position(const Position& other)
{
}


Position::~Position()
{
}

//The SetFrameTime function is used to set the frame speed in this class. Position will use that frame time speed to calculate how fast the viewer should be moving and rotating.
//This function should always be called at the beginning of each frame, before using this class to move the viewing position
void Position::SetFrameTime(float time)
{
	this->m_frameTime = time;
}

//GetRotation returns the Y-axis rotation of the viewer. This is only helper function we need for this tutorial but could be expanded to get more information about the location of the viewer
void Position::GetRotation(float& rotationY)
{
	rotationY = this->m_rotationY;
}

//The movement functions both work the same. Both functions are called each frame. The key down input variable to each function indicates if the user is pressing 
//the left key or the right key. If they are pressing the key then each frame the speed will accelerate until it hits a maximum. 
//This way the camera speeds up similar to the acceleration in a vehicle creating the effect of smooth movement and high responsiveness. 
//Likewise if the user releases the key and the key down variable is false it will then smoothly slow down each frame until the speed hits zero. 
//The speed is calculated against the frame time to ensure the movement speed remains the same regardless of the frame rate. 
//Each function then uses some basic math to calculate the new position of the camera.
void Position::TurnLeft(bool keydown)
{
	//If the key is pressed increase the speed at which the camera turn left. Else slow down the turn speed
	if (keydown)
	{
		this->m_leftTurnSpeed += this->m_frameTime * 0.01f;

		if (this->m_leftTurnSpeed > (this->m_frameTime * 0.15f))
		{
			this->m_leftTurnSpeed = this->m_frameTime * 0.15f;
		}
	}
	else
	{
		this->m_leftTurnSpeed -= this->m_frameTime * 0.005f;
		if (this->m_leftTurnSpeed < 0.0f)
		{
			this->m_leftTurnSpeed = 0.0f;
		}
	}

	//Update the rotation using the turning speed
	this->m_rotationY -= this->m_leftTurnSpeed;
	if (this->m_rotationY < 0.0f)
	{
		this->m_rotationY += 360.0f;
	}
}

void Position::TurnRight(bool keydown)
{
	//If the key is pressed increase the speed at which the camera turns right. Else slow down the turn speed
	if (keydown)
	{
		this->m_rightTurnSpeed += this->m_frameTime * 0.01f;

		if (this->m_rightTurnSpeed > (this->m_frameTime * 0.15f))
		{
			this->m_rightTurnSpeed = this->m_frameTime * 0.15f;
		}
	}
	else
	{
		this->m_rightTurnSpeed -= this->m_frameTime * 0.005f;
		
		if (this->m_rightTurnSpeed < 0.0f)
		{
			this->m_rightTurnSpeed = 0.0f;
		}
	}

	//Update the rotation using the turning speed
	this->m_rotationY += this->m_rightTurnSpeed;
	if (this->m_rotationY > 360.0f)
	{
		this->m_rotationY -= 360.0f;
	}
}