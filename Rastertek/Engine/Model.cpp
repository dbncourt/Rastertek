////////////////////////////////////////////////////////////////////////////////
// Filename: Model.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Model.h"


Model::Model()
{
	this->m_vertexBuffer = nullptr;
	this->m_indexBuffer = nullptr;
	this->m_model = nullptr;
	this->m_TextureArray = nullptr;
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* baseTextureFileName, WCHAR* bumpMapTextureFileName, WCHAR* specularMapTextureFileName)
{
	bool result;

	// Load in the model data
	result = Model::LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	//Calculate the normal, tangent and binormal vectors for the model.
	Model::CalculateModelVectors();

	// Initialize the vertex and index buffers.
	result = Model::InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = Model::LoadTextures(device, baseTextureFileName, bumpMapTextureFileName, specularMapTextureFileName);
	if(!result)
	{
		return false;
	}

	return true;
}

void Model::Shutdown()
{
	//Release the model texture
	Model::ReleaseTextures();

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

ID3D11ShaderResourceView** Model::GetTextureArray()
{
	return this->m_TextureArray->GetTextureArray();
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
		vertices[i].tangent = this->m_model[i].tanget;
		vertices[i].binormal = this->m_model[i].binormal;

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

bool Model::LoadTextures(ID3D11Device* device, WCHAR* baseTextureFileName, WCHAR* bumpMapTextureFileName, WCHAR* specularMapTextureFileName)
{
	bool result;

	//Create the texture object
	this->m_TextureArray = new TextureArray();
	if (!this->m_TextureArray)
	{
		return false;
	}

	//Initialize the texture object
	result = this->m_TextureArray->Initialize(device, baseTextureFileName, bumpMapTextureFileName, specularMapTextureFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::ReleaseTextures()
{
	//Release the texture object
	if (!this->m_TextureArray)
	{
		this->m_TextureArray->Shutdown();
		delete this->m_TextureArray;
		this->m_TextureArray = nullptr;
	}
}

bool Model::LoadModel(char* modelFilename)
{
	ifstream fIn;
	char input;

	//Open the model file
	fIn.open(modelFilename);

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

void Model::CalculateModelVectors()
{
	//Calculate the number of faces in the model
	ULONG faceCount = this->m_vertexCount / 3;


	//Go through all the faces and calculate the tangent, binormal and normal vectors.
	for (ULONG i = 0, index = 0; i < faceCount; i++)
	{
		TempVertexType vertex1;
		TempVertexType vertex2;
		TempVertexType vertex3;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
		D3DXVECTOR3 normal;

		//Get the three vertices for this face from the model
		vertex1.position = this->m_model[index].position;
		vertex1.texture = this->m_model[index].texture;
		vertex1.normal = this->m_model[index].normal;
		index++;

		vertex2.position = this->m_model[index].position;
		vertex2.texture = this->m_model[index].texture;
		vertex2.normal = this->m_model[index].normal;
		index++;

		vertex3.position = this->m_model[index].position;
		vertex3.texture = this->m_model[index].texture;
		vertex3.normal = this->m_model[index].normal;
		index++;

		//Calculate the tangent and binormal of that face
		Model::CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		//Calculate the new normal using the tangent and binormal
		Model::CalculateNormal(tangent, binormal, normal);

		//Store the normal, tangent and binormal for this face back in the model structure
		this->m_model[index - 1].normal = normal;
		this->m_model[index - 1].tanget = tangent;
		this->m_model[index - 1].binormal = binormal;

		this->m_model[index - 2].normal = normal;
		this->m_model[index - 2].tanget = tangent;
		this->m_model[index - 2].binormal = binormal;

		this->m_model[index - 3].normal = normal;
		this->m_model[index - 3].tanget = tangent;
		this->m_model[index - 3].binormal = binormal;
	}

}

void Model::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, D3DXVECTOR3& tangent, D3DXVECTOR3& binormal)
{

	D3DXVECTOR3 vector1;
	D3DXVECTOR3 vector2;
	D3DXVECTOR2 tuVector;
	D3DXVECTOR2 tvVector;
	float denominator;
	float length;

	//Calculate the two vectors for this face
	vector1 = vertex2.position - vertex1.position;
	vector2 = vertex3.position - vertex1.position;

	//Calculate the tu and tv texture space vector
	tuVector.x = vertex2.texture.x - vertex1.texture.x;
	tvVector.x = vertex2.texture.y - vertex1.texture.y;

	tuVector.y = vertex3.texture.x - vertex1.texture.x;
	tvVector.y = vertex3.texture.y - vertex1.texture.y;

	//Calculate the denominator of the tangent/binormal equation
	denominator = 1.0f / ((tuVector.x * tvVector.y) - (tuVector.y * tvVector.x));

	//Calculate the cross products and multiply by the coefficient to get the tangent and binormal
	tangent.x = ((tvVector.y * vector1.x) - (tvVector.x * vector2.x)) * denominator;
	tangent.y = ((tvVector.y * vector1.y) - (tvVector.x * vector2.y)) * denominator;
	tangent.z = ((tvVector.y * vector1.z) - (tvVector.x * vector2.z)) * denominator;

	binormal.x = ((tuVector.x * vector2.x) - (tuVector.y * vector1.x)) * denominator;
	binormal.y = ((tuVector.x * vector2.y) - (tuVector.y * vector1.y)) * denominator;
	binormal.z = ((tuVector.x * vector2.z) - (tuVector.y * vector1.z)) * denominator;

	//Calculate the length of this normal
	length = sqrt((tangent.x * tangent.x) +(tangent.y * tangent.y) + (tangent.z * tangent.z));

	//Normalize the normal and then store it
	tangent.x /= length;
	tangent.y /= length;
	tangent.z /= length;

	//Calculate the length of this normal
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	//Normalize the normal and then store it
	binormal.x /= length;
	binormal.y /= length;
	binormal.z /= length;
}

void Model::CalculateNormal(D3DXVECTOR3 tangent, D3DXVECTOR3 binormal, D3DXVECTOR3& normal)
{
	float length;

	//Calculate the cross product of the tangent and binormal which will give the normal vector
	normal.x = ((tangent.y * binormal.z) - (tangent.z * binormal.y));
	normal.y = ((tangent.z * binormal.x) - (tangent.x * binormal.z));
	normal.z = ((tangent.x * binormal.y) - (tangent.y * binormal.x));

	//Calculate the length of the normal
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	//Normalize the normal
	normal.x /= length;
	normal.y /= length;
	normal.z /= length;
}
