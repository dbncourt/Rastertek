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
	D3DXCOLOR m_ambientColor;
	D3DXCOLOR m_diffuseColor;
	D3DXVECTOR3 m_lightDirection;

public:
	Light();
	Light(const Light& other);
	~Light();

	D3DXCOLOR GetAmbientColor();
	void SetAmbientColor(D3DXCOLOR ambientColor);

	D3DXCOLOR GetDiffuseColor();
	void SetDiffuseColor(D3DXCOLOR diffuseColor);

	D3DXVECTOR3 GetLightDirection();
	void SetLightDirection(D3DXVECTOR3 lightDirection);
};
#endif