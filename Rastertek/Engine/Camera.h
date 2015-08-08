////////////////////////////////////////////////////////////////////////////////
// Filename: Camera.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERA_H_
#define _CAMERA_H_

//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: Camera
////////////////////////////////////////////////////////////////////////////////
class Camera
{
private:
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_rotation;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_reflectionViewMatrix;

public:
	Camera();
	Camera(const Camera& other);
	~Camera();

	void SetPosition(D3DXVECTOR3 position);
	D3DXVECTOR3 GetPosition();

	void SetRotation(D3DXVECTOR3 rotation);
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX& viewMatrix);

	void RenderReflection(float height);
	void GetReflectionViewMatrix(D3DXMATRIX& reflectionMatrix);
};
#endif