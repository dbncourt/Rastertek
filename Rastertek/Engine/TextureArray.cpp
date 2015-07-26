////////////////////////////////////////////////////////////////////////////////
// Filename: TextureArray.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureArray.h"


TextureArray::TextureArray()
{
	this->m_textures[0] = nullptr;
	this->m_textures[1] = nullptr;
}

TextureArray::TextureArray(const TextureArray& other)
{
}


TextureArray::~TextureArray()
{
}

bool TextureArray::Initialize(ID3D11Device* device, WCHAR* baseTextureFileName, WCHAR* colorTextureFileName, WCHAR* alphaTextureFileName)
{
	HRESULT result;

	//Load the first texture in
	result = D3DX11CreateShaderResourceViewFromFile(device, baseTextureFileName, nullptr, nullptr, &this->m_textures[0], nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//Load the second texture in
	result = D3DX11CreateShaderResourceViewFromFile(device, colorTextureFileName, nullptr, nullptr, &this->m_textures[1], nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//Load the third texture in
	result = D3DX11CreateShaderResourceViewFromFile(device, alphaTextureFileName, nullptr, nullptr, &this->m_textures[2], nullptr);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureArray::Shutdown()
{
	//Release the texture resource
	if (this->m_textures[0])
	{
		this->m_textures[0]->Release();
		this->m_textures[0] = nullptr;
	}

	if (this->m_textures[1])
	{
		this->m_textures[1]->Release();
		this->m_textures[1] = nullptr;
	}

	if (this->m_textures[2])
	{
		this->m_textures[2]->Release();
		this->m_textures[2] = nullptr;
	}
}

ID3D11ShaderResourceView** TextureArray::GetTextureArray()
{
	return this->m_textures;
}