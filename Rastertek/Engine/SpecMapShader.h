////////////////////////////////////////////////////////////////////////////////
// Filename: SpecMapShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SPECMAPSHADER_H_
#define _SPECMAPSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: SpecMapShader
////////////////////////////////////////////////////////////////////////////////
class SpecMapShader
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
		D3DXCOLOR specularColor;
		float specularPower;
		D3DXVECTOR3 lightDirection;
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;

public:
	SpecMapShader();
	SpecMapShader(const SpecMapShader& other);
	~SpecMapShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDirection, D3DXCOLOR diffuseColor, D3DXVECTOR3 cameraPosition, D3DXCOLOR specularColor, float specularPower);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDirection, D3DXCOLOR diffuseColor, D3DXVECTOR3 cameraPosition, D3DXCOLOR specularColor, float specularPower);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};

#endif