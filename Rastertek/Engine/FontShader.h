////////////////////////////////////////////////////////////////////////////////
// Filename: FontShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONTSHADER_H_
#define _FONTSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: FontShader
////////////////////////////////////////////////////////////////////////////////

class FontShader
{

private:
	struct ConstantBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct PixelBufferType
	{
		D3DXCOLOR pixelColor;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11Buffer* m_constantBuffer;
	ID3D11Buffer* m_pixelBuffer;
	ID3D11SamplerState* m_samplerState;

public:
	FontShader();
	FontShader(const FontShader& other);
	~FontShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR fontColor);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
	void ShutdownShaders();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR fontColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};

#endif