////////////////////////////////////////////////////////////////////////////////
// Filename: Model.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODEL_H_
#define _MODEL_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Texture.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Model
////////////////////////////////////////////////////////////////////////////////
class Model
{

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	UINT m_vertexCount;
	UINT m_indexCount;
	Texture* m_Texture;
	ModelType* m_model;

public:
	Model();
	Model(const Model& other);
	~Model();

	bool Initialize(ID3D11Device* device, char* modelFileName, WCHAR* textureFileName);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTexture(ID3D11Device* device, WCHAR* textureFileName);
	void ReleaseTexture();

	bool LoadModel(char* modelFileName);
	void ReleaseModel();
};
#endif