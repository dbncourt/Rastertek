////////////////////////////////////////////////////////////////////////////////
// Filename: BumpMapShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BUMPMAPSHADER_H_
#define _BUMPMAPSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: BumpMapShader
////////////////////////////////////////////////////////////////////////////////
class BumpMapShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX worldMatrix;
		D3DXMATRIX viewMatrix;
		D3DXMATRIX projectionMatrix;
	};

	struct LightBufferType
	{
		D3DXCOLOR diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

public:
	BumpMapShader();
	BumpMapShader(const BumpMapShader& other);
	~BumpMapShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDireciton, D3DXCOLOR diffuseColor);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
	void ShutdownShaders();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDireciton, D3DXCOLOR diffuseColor);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};

#endif