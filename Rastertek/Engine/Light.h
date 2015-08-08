////////////////////////////////////////////////////////////////////////////////
// Filename: Light.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHT_H_
#define _LIGHT_H_

//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: Light
////////////////////////////////////////////////////////////////////////////////
class Light
{
private:
	D3DXCOLOR m_diffuseColor;
	D3DXVECTOR4 m_lightDirection;

public:
	Light();
	Light(const Light& other);
	~Light();

	void SetDiffuseColor(D3DXCOLOR diffuseColor);
	D3DXCOLOR GetDiffuseColor();

	void SetPosition(D3DXVECTOR4 position);
	D3DXVECTOR4 GetPosition();
};
#endif