////////////////////////////////////////////////////////////////////////////////
// Filename: Text.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXT_H_
#define _TEXT_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Font.h"
#include "FontShader.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Text
////////////////////////////////////////////////////////////////////////////////
class Text
{
private:
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		UINT vertexCount;
		UINT indexCount;
		int maxLength;
		float red;
		float green;
		float blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	Font* m_Font;
	FontShader* m_FontShader;
	int m_screenWidth;
	int m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;

	SentenceType* m_sentence;

public:
	Text();
	Text(const Text& other);
	~Text();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix);

	bool SetRenderCount(int renderCount, ID3D11DeviceContext* deviceContext);

private:
	bool InitializeSentence(SentenceType** sentence, int indexCount, ID3D11Device* device);
	bool UpdateSentence(SentenceType* sentence, char* text, D3DXVECTOR2 position, D3DXCOLOR color, ID3D11DeviceContext* deviceContext);
	void ReleaseSentence(SentenceType** sentece);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix);
};
#endif