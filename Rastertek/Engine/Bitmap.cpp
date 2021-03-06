////////////////////////////////////////////////////////////////////////////////
// Filename: Bitmap.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Bitmap.h"


Bitmap::Bitmap()
{
	this->m_vertexBuffer = nullptr;
	this->m_indexBuffer = nullptr;
}

Bitmap::Bitmap(const Bitmap& other)
{
}


Bitmap::~Bitmap()
{
}


bool Bitmap::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	bool result;

	//Store the screen size
	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;

	//Store the size in pixels that this bitmap should be rendered at
	this->m_bitmapWidth = bitmapWidth;
	this->m_bitmapHeight = bitmapHeight;

	//Initialize the previous rendering position to negative one
	this->m_previousPosX = -1;
	this->m_previousPosY = -1;

	//Initialize the vertex and index buffers.
	result = Bitmap::InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void Bitmap::Shutdown()
{
	// Shutdown the vertex and index buffers.
	Bitmap::ShutdownBuffers();
}

bool Bitmap::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	//Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen
	result = Bitmap::UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}

	//Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	Bitmap::RenderBuffers(deviceContext);
	
	return true;
}

UINT Bitmap::GetIndexCount()
{
	return this->m_indexCount;
}

bool Bitmap::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;
	VertexType* vertices;
	UINT* indices;

	//Set the number of vertices in the vertex array
	this->m_vertexCount = 6;

	//Set the number of indices in the index array
	this->m_indexCount = this->m_vertexCount;

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

	//Initialize vertex array to zeros at first
	ZeroMemory(vertices, sizeof(VertexType)*this->m_vertexCount);

	//Load the index array with data
	for (UINT i = 0; i < this->m_indexCount; i++)
	{
		indices[i] = i;
	}

	//Set up the description of the static vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * this->m_vertexCount;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(UINT) * this->m_indexCount;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Now create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &this->m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Bitmap::ShutdownBuffers()
{
	if (this->m_vertexBuffer)
	{
		this->m_vertexBuffer->Release();
		this->m_vertexBuffer = nullptr;
	}

	if (this->m_indexBuffer)
	{
		this->m_indexBuffer->Release();
		this->m_indexBuffer = nullptr;
	}
}

bool Bitmap::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	HRESULT result;

	float left;
	float right;
	float top;
	float bottom;

	VertexType* vertices;
	VertexType* verticesPtr;

	//If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it currently has the correct parameters
	if (positionX == this->m_previousPosX && positionY == this->m_previousPosY)
	{
		return true;
	}

	//If it has changed then update the position it is being rendered to
	this->m_previousPosX = positionX;
	this->m_previousPosY = positionY;

	//Calculate the screen coordinates of the left side of the bitmap
	left = (float)((this->m_screenWidth / 2) * -1) + (float)this->m_previousPosX;

	//Calculate the screen coordinates of the right side of the bitmap
	right = left + (float)this->m_bitmapWidth;

	//Calculate the screen coordinates of the top of the bitmap
	top = (float)(this->m_screenHeight / 2) - (float)this->m_previousPosY;

	//Calculate the screen coordinates of the bottom of the bitmap
	bottom = top - (float)this->m_bitmapHeight;

	//Create the vertex array
	vertices = new VertexType[this->m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//Load the vertex array with data
	//First triangle
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);//Top Left
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);//Bottom Right
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);//Bottom Left
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	//Second triangle
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);//Top Left
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);//Top Right
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);//Bottom Right
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	//Lock the vertex buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	result = deviceContext->Map(this->m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}
	//Get a pointer to the data in the vertex buffer
	verticesPtr = (VertexType*)mappedSubresource.pData;
	//Copy the data into the vertex buffer
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * this->m_vertexCount));
	//Unlock the vertex buffer
	deviceContext->Unmap(this->m_vertexBuffer, 0);

	//Release the vertex array as it is no longer needed
	delete[] vertices;
	vertices = nullptr;

	return true;
}

void Bitmap::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride;
	UINT offset;

	//Set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &this->m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(this->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case, triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}