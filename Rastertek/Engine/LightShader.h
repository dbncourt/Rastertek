////////////////////////////////////////////////////////////////////////////////
// Filename: LightShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

/////////////
// GLOBALS //
/////////////
const int NUM_LIGHTS = 4;

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightShader
////////////////////////////////////////////////////////////////////////////////
class LightShader
{

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightColorBufferType
	{
		D3DXCOLOR diffuseColor[NUM_LIGHTS];
	};

	struct LightPositionBufferType
	{
		D3DXVECTOR4 lightPosition[NUM_LIGHTS];
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightColorBuffer;
	ID3D11Buffer* m_lightPositionBuffer;

public:
	LightShader();
	LightShader(const LightShader& other);
	~LightShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR diffuseColor[], D3DXVECTOR4 lightPosition[]);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFileName, WCHAR* pixelShaderFileName);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR diffuseColor[], D3DXVECTOR4 lightPosition[]);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};
#endif