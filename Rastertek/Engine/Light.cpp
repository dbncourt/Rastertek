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

D3DXCOLOR Light::GetAmbientColor()
{
	return this->m_ambientColor;
}

void Light::SetAmbientColor(D3DXCOLOR ambientColor)
{
	this->m_ambientColor = ambientColor;
}

D3DXCOLOR Light::GetDiffuseColor()
{
	return this->m_diffuseColor;
}

void Light::SetDiffuseColor(D3DXCOLOR diffuseColor)
{
	this->m_diffuseColor = diffuseColor;
}

D3DXVECTOR3 Light::GetLightDirection()
{
	return this->m_lightDirection;
}

void Light::SetLightDirection(D3DXVECTOR3 lightDirection)
{
	this->m_lightDirection = lightDirection;
}