////////////////////////////////////////////////////////////////////////////////
// Filename: Position.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POSITION_H_
#define _POSITION_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: Position
////////////////////////////////////////////////////////////////////////////////

class Position
{
private:
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_rotation;
	float m_frameTime;
	float m_leftSpeed;
	float m_rightSpeed;

public:
	Position();
	Position(const Position& other);
	~Position();

	void SetPosition(_In_ D3DXVECTOR3 position);
	void GetPosition(_Out_ D3DXVECTOR3& position);

	void SetRotation(_In_ D3DXVECTOR3 rotation);
	void GetRotation(_Out_ D3DXVECTOR3 rotation);

	void SetFrameTime(float time);

	void MoveLeft(bool keydown);
	void MoveRight(bool keydown);
};
#endif