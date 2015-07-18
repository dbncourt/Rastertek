////////////////////////////////////////////////////////////////////////////////
// Filename: Light.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Light.h"


Light::Light()
{
}

Light::Light(const Light& other)
{
}

Light::~Light()
{
}

void Light::SetDiffuseColor(D3DXCOLOR diffuseColor)
{
	this->m_diffuseColor = diffuseColor;
}

void Light::SetLightDirection(D3DXVECTOR3 lightDirection)
{
	this->m_lightDirection = lightDirection;
}

D3DXCOLOR Light::GetDiffuseColor()
{
	return this->m_diffuseColor;
}

D3DXVECTOR3 Light::GetLightDirection()
{
	return this->m_lightDirection;
}