////////////////////////////////////////////////////////////////////////////////
// Filename: WaterShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _WATERSHADER_H_
#define _WATERSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: WaterShader
////////////////////////////////////////////////////////////////////////////////
class WaterShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct ReflectionBufferType
	{
		D3DXMATRIX reflection;
	};

	struct WaterBufferType
	{
		float waterTranslation;
		float reflectRefractScale;
		D3DXVECTOR2 padding;
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_reflectionBuffer;
	ID3D11Buffer* m_waterBuffer;

public:
	WaterShader();
	WaterShader(const WaterShader& other);
	~WaterShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX reflectionViewMatrix, ID3D11ShaderResourceView* reflectionTexture, ID3D11ShaderResourceView* refractionTexture, ID3D11ShaderResourceView* normalTexture, float waterTranslation, float reflectRefractScale);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFileName, WCHAR* pixelShaderFileName);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX reflectionViewMatrix, ID3D11ShaderResourceView* reflectionTexture, ID3D11ShaderResourceView* refractionTexture, ID3D11ShaderResourceView* normalTexture, float waterTranslation, float reflectRefractScale);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
};

#endif