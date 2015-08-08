////////////////////////////////////////////////////////////////////////////////
// Filename: Model.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Model.h"


Model::Model()
{
	this->m_vertexBuffer = nullptr;
	this->m_indexBuffer = nullptr;
	this->m_Texture = nullptr;
	this->m_model = nullptr;
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* device, char* modelFileName, WCHAR* textureFileName)
{
	bool result;

	// Load in the model data
	result = Model::LoadModel(modelFileName);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = Model::InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = Model::LoadTexture(device, textureFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
	//Release the model texture
	Model::ReleaseTexture();

	// Shutdown the vertex and index buffers
	Model::ShutdownBuffers();

	//Release the model data
	Model::ReleaseModel();
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	//Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	Model::RenderBuffers(deviceContext);
}

int Model::GetIndexCount()
{
	return this->m_indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return this->m_Texture->GetTexture();
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	UINT* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	//Create the vertex array
	vertices = new VertexType[this->m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//Create the index array
	indices = new UINT[this->m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data
	for (UINT i = 0; i < this->m_vertexCount; i++)
	{
		vertices[i].position = this->m_model[i].position;
		vertices[i].texture = this->m_model[i].texture;
		vertices[i].normal = this->m_model[i].normal;

		indices[i] = i;
	}

	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * this->m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the sub-resource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the sub-resource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &this->m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Release the arrays now that the vertex and index buffers have been created and loaded
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::ShutdownBuffers()
{
	//Release the index buffer
	if (this->m_indexBuffer)
	{
		this->m_indexBuffer->Release();
		this->m_indexBuffer = nullptr;
	}

	//Release the vertex buffer
	if (this->m_vertexBuffer)
	{
		this->m_vertexBuffer->Release();
		this->m_vertexBuffer = nullptr;
	}
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//Set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &this->m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(this->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::LoadTexture(ID3D11Device* device, WCHAR* fileName)
{
	bool result;

	//Create the texture object
	this->m_Texture = new Texture;
	if (!this->m_Texture)
	{
		return false;
	}

	//Initialize the texture object
	result = this->m_Texture->Initialize(device, fileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::ReleaseTexture()
{
	//Release the texture object
	if (this->m_Texture)
	{
		this->m_Texture->Shutdown();
		delete this->m_Texture;
		this->m_Texture = nullptr;
	}
}

bool Model::LoadModel(char* modelFileName)
{
	ifstream fIn;
	char input;

	//Open the model file
	fIn.open(modelFileName);

	// If it could not open the file then exit
	if (fIn.fail())
	{
		return false;
	}
	//Read up to the value of vertex count
	fIn.get(input);
	while (input != ':')
	{
		fIn.get(input);
	}

	//Read in the vertex count
	fIn >> this->m_vertexCount;

	//Set the number of indices to be the same as the vertex count
	this->m_indexCount = this->m_vertexCount;

	//Create the model using the vertex count that was read in
	this->m_model = new ModelType[this->m_vertexCount];
	if (!this->m_model)
	{
		return false;
	}

	//Read up to the beginning of the data
	fIn.get(input);
	while (input != ':')
	{
		fIn.get(input);
	}
	fIn.get(input);
	fIn.get(input);

	//Read in the vertex data.
	for (UINT i = 0; i < this->m_vertexCount; i++)
	{
		fIn >> this->m_model[i].position.x >> this->m_model[i].position.y >> this->m_model[i].position.z;
		fIn >> this->m_model[i].texture.x >> this->m_model[i].texture.y;
		fIn >> this->m_model[i].normal.x >> this->m_model[i].normal.y >> this->m_model[i].normal.z;
	}

	//Close the model file
	fIn.close();

	return true;
}

void Model::ReleaseModel()
{
	if (this->m_model)
	{
		delete[] this->m_model;
		this->m_model = nullptr;
	}
}