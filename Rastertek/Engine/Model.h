////////////////////////////////////////////////////////////////////////////////
// Filename: Model.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODEL_H_
#define _MODEL_H_

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
#include "TextureArray.h"

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
		D3DXVECTOR3 tanget;
		D3DXVECTOR3 binormal;
	};

	struct ModelType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tanget;
		D3DXVECTOR3 binormal;
	};

	struct TempVertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	ModelType* m_model;
	TextureArray* m_TextureArray;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	UINT m_vertexCount;
	UINT m_indexCount;


public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device* device, char* modelFilename, WCHAR* baseTextureFilename, WCHAR* bumpMapTextureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
	ID3D11ShaderResourceView** GetTextureArray();

private :
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTexture(ID3D11Device* device, WCHAR* baseTextureFilename, WCHAR* bumpMapTextureFilename);
	void ReleaseTexture();

	bool LoadModel(char* modelFilename);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangetBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, D3DXVECTOR3& tangent, D3DXVECTOR3& binormal);
	void CalculateNormal(D3DXVECTOR3 tangent, D3DXVECTOR3 binormal, D3DXVECTOR3& normal);
};
#endif