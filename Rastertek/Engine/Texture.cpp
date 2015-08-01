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

bool Texture::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	//Load the texture in
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, nullptr, nullptr, &this->m_texture, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	// Release the texture resource.
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