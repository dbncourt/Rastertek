////////////////////////////////////////////////////////////////////////////////
// Filename: Font.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONT_H_
#define _FONT_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Texture.h"

class Font
{
private:
	struct FontType
	{
		float left;
		float right;
		int size;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	FontType* m_Font;
	Texture* m_Texture;

	int m_charactersCount;

public:
	Font();
	Font(const Font& other);
	~Font();

	bool Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void* vertices, char* sentence, D3DXVECTOR2 drawPosition);

private:
	bool LoadFontData(char* fontFilename);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* device, WCHAR* textureFilename);
	void ReleaseTexture();
};

#endif