////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Texture.h"


Texture::Texture()
{
	this->m_texture = nullptr;
}

Texture::Texture(const Texture& other)
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, WCHAR* fileName)
{
	HRESULT result;

	//Load the texture in
	result = D3DX11CreateShaderResourceViewFromFile(device, fileName, nullptr, nullptr, &this->m_texture, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	// Release the ID3D11ShaderResourceView.
	if (this->m_texture)
	{
		this->m_texture->Release();
		this->m_texture = nullptr;
	}
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return this->m_texture;
}