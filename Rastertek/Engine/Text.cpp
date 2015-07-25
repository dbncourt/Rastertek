///////////////////////////////////////////////////////////////////////////////
// Filename: Text.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Text.h"


Text::Text()
{
	this->m_Font = nullptr;
	this->m_FontShader = nullptr;

	this->m_sentence1 = nullptr;
	this->m_sentence2 = nullptr;
}

Text::Text(const Text& other)
{
}


Text::~Text()
{
}

bool Text::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	bool result;

	//Store the screen width and height
	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;

	//Store the base view matrix
	this->m_baseViewMatrix = baseViewMatrix;

	//Create a font object
	this->m_Font = new Font();
	if (!this->m_Font)
	{
		return false;
	}

	//Initialize the Font object
	result = this->m_Font->Initialize(device, "fontdata.txt", L"font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Font object", L"Error", MB_OK);
		return false;
	}

	//Create the FontShader object
	this->m_FontShader = new FontShader();
	if (!this->m_FontShader)
	{
		return false;
	}

	//Initialize the FontShader object
	result = this->m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the FontShader object", L"Error", MB_OK);
		return false;
	}

	//Initialize the first sentence
	result = InitializeSentence(&this->m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	//Now update the sentence vertex buffer with the new string information
	result = UpdateSentence(this->m_sentence1, "Hello", D3DXVECTOR2(100, 100), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), deviceContext);
	if (!result)
	{
		return false;
	}

	//Initialize the second sentence
	result = InitializeSentence(&this->m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	//Now update the sentence vertex buffer with the new string information
	result = UpdateSentence(this->m_sentence2, "Goodbye", D3DXVECTOR2(100, 200), D3DXCOLOR(1.0f, 1.0f, .0f, 1.0f), deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void Text::Shutdown()
{
	//Release the first Sentence
	Text::ReleaseSentence(&this->m_sentence1);

	//Release the second Sentence
	Text::ReleaseSentence(&this->m_sentence2);

	//Release the FontShader object
	if (this->m_FontShader)
	{
		this->m_FontShader->Shutdown();
		delete this->m_FontShader;
		this->m_FontShader = nullptr;
	}

	//Release the Font object
	if (this->m_Font)
	{
		this->m_Font->Shutdown();
		delete this->m_Font;
		this->m_Font = nullptr;
	}
}

bool Text::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	//Draw the first sentence
	result = RenderSentence(deviceContext, this->m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	//Draw the second sentence
	result = RenderSentence(deviceContext, this->m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Text::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	HRESULT result;

	VertexType* vertices;
	UINT* indices;
	
	//Create a new sentence object
	*sentence = new SentenceType();
	if (!*sentence)
	{
		return false;
	}

	//Initialize the sentence buffers to null
	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;

	//Set the maximum length of the sentence
	(*sentence)->maxLength = maxLength;

	//Set the number of vertices in the vertex array
	(*sentence)->vertexCount = 6 * maxLength;

	//Set the number of indices in the index array
	(*sentence)->indexCount = (*sentence)->vertexCount;

	//Create the vertex array
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	//Create the index array
	indices = new UINT[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	//Initialize vertex array to zeros at first
	ZeroMemory(vertices, sizeof(VertexType) * (*sentence)->vertexCount);

	//Initialize the index array
	for (UINT i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	//Create the Vertex Buffer Descriptor
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Set up the description of the dynamic vertex buffer
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the sub resource to map the data
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));

	//Give the sub resource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Create the Index Buffer Descriptor
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the static index buffer
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(UINT) * (*sentence)->indexCount;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	//Create the sub resource to map the data
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));

	//Give the sub-resource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
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

bool Text::UpdateSentence(SentenceType* sentence, char* text, D3DXVECTOR2 position, D3DXCOLOR color, ID3D11DeviceContext* deviceContext)
{
	HRESULT result;

	int numLetters;
	VertexType* vertices;
	VertexType* verticesPtr;
	D3DXVECTOR2 drawPosition;

	//Store the color of the sentence
	sentence->red = color.r;
	sentence->green = color.g;
	sentence->blue = color.b;

	//Get the number of letters in the sentence
	numLetters = (int)strlen(text);

	//Check for possible buffer overflow
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	//Create the vertex array
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	//Initialize vertex array to zeros at first
	ZeroMemory(vertices, sizeof(VertexType) * sentence->vertexCount);

	//Calculate the X and Y pixel position on the screen to start drawing to.
	drawPosition.x = (float)(((this->m_screenWidth / 2) * -1) + position.x);
	drawPosition.y = (float)((this->m_screenHeight / 2) - position.y);

	//Use the font class to build the vertex array from the sentence text and sentence draw location
	this->m_Font->BuildVertexArray((void*)vertices, text, drawPosition);

	//MappedSubresource to write the data on the video card memory
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//Lock the vertex buffer so it can be written to
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the vertex buffer
	verticesPtr = (VertexType*)mappedSubresource.pData;
	
	//Copy the data into the vertex buffer
	memcpy(verticesPtr, (void*)vertices, sizeof(VertexType)*sentence->vertexCount);

	//Unlock the vertex buffer
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	//Release the vertex array as it is no longer needed
	delete[] vertices;
	vertices = nullptr;

	return true;
}

void Text::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		//Release the sentence vertex buffer
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = nullptr;
		}

		//Release the sentence index buffer
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = nullptr;
		}

		//Release the sentence
		delete *sentence;
		*sentence = nullptr;
	}
}

bool Text::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	UINT stride;
	UINT offset;
	
	//Set the vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create a pixel color vector with the input sentence color
	D3DXCOLOR pixelColor = D3DXCOLOR(sentence->red, sentence->green, sentence->blue, 1.0f);

	//Render the text using the font shader
	result = this->m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, this->m_baseViewMatrix, orthoMatrix, this->m_Font->GetTexture(), pixelColor);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Text::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	bool result;

	char tempString[16];
	char fpsString[16];
	D3DXCOLOR color;

	//Truncate the fps to below 10,000
	if (fps > 9999)
	{
		fps = 9999;
	}

	//Convert the fps integer to string format
	_itoa_s(fps, tempString, 10);

	//Setup the fps string
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	//If fps is 60 or above set the fps color to green
	if (fps >= 60)
	{
		color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	}
	//If fps is less than 60 and more than 30 then set the color to yellow
	else if (fps < 60 && fps > 30)
	{ 
		color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	}
	//If fps is less than 30 then set the color to red
	else
	{
		color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	}
	
	//Update the sentence vertex buffer with the new string information
	result = Text::UpdateSentence(this->m_sentence1, fpsString, D3DXVECTOR2(20.0f, 20.0f), color, deviceContext);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Text::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	bool result;

	char tempString[16];
	char cpuString[16];

	//Convert the cpu integer to string format
	_itoa_s(cpu, tempString, 10);

	//Setup the cpu string
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	//Update the sentence vertex buffer with the new string information
	result = Text::UpdateSentence(this->m_sentence2, cpuString, D3DXVECTOR2(20.0f, 40.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), deviceContext);
	if (!result)
	{
		return false;
	}
	return true;
}
