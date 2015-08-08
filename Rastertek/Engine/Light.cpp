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

D3DXCOLOR Light::GetDiffuseColor()
{
	return this->m_diffuseColor;
}

void Light::SetPosition(D3DXVECTOR4 lightDirection)
{
	this->m_lightDirection = lightDirection;
}

D3DXVECTOR4 Light::GetPosition()
{
	return this->m_lightDirection;
}