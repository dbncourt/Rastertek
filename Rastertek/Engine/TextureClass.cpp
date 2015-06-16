////////////////////////////////////////////////////////////////////////////////
// Filename: TextureClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureClass.h"


TextureClass::TextureClass()
{
	this->m_Texture = nullptr;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	//Load the texture in
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, nullptr, nullptr, &this->m_Texture, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (this->m_Texture)
	{
		this->m_Texture->Release();
		this->m_Texture = nullptr;
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return this->m_Texture;
}