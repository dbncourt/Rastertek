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

D3DXVECTOR3 Light::GetDirection()
{
	return this->m_lightDirection;
}

void Light::SetDirection(D3DXVECTOR3 lightDirection)
{
	this->m_lightDirection = lightDirection;
}

D3DXCOLOR Light::GetSpecularColor()
{
	return this->m_specularColor;
}

void Light::SetSpecularColor(D3DXCOLOR specularColor)
{
	this->m_specularColor = specularColor;
}

float Light::GetSpecularPower()
{
	return this->m_specularPower;
}

void Light::SetSpecularPower(float specularPower)
{
	this->m_specularPower = specularPower;
}

