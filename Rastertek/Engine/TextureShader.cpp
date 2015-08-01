////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureShader.h"

TextureShader::TextureShader()
{
	this->m_vertexShader = nullptr;
	this->m_pixelShader = nullptr;
	this->m_layout = nullptr;
	this->m_sampleState = nullptr;
	this->m_matrixBuffer = nullptr;
}

TextureShader::TextureShader(const TextureShader& other)
{
}

TextureShader::~TextureShader()
{
}

bool TextureShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex and pixel shaders
	result = TextureShader::InitializeShader(device, hwnd, L"TextureVertexShader.hlsl", L"TexturePixelShader.hlsl");
	if (!result)
	{
		return false;
	}
	return true;
}

void TextureShader::Shutdown()
{
	//Shutdown the vertex and pixel shaders as well as the related objects
	TextureShader::ShutdownShader();
}

bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = TextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffer with the shader
	TextureShader::RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	//Initialize the pointers this function will use to null
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//Compile the vertex shader code
	result = D3DX11CompileFromFile(vsFileName, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		//If the shader failed to compile, it should have written something to the error message
		if (errorMessage)
		{
			TextureShader::OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		//If there was nothing in the error message then it simply could not find the shader file itself
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//Compile the pixel shader code
	result = D3DX11CompileFromFile(psFileName, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			TextureShader::OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		//If there was nothing in the error message then it simply could not find the file itself
		else
		{
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//Create the vertex shader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &this->m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create the pixel shader from the buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &this->m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create the vertex input layout description
	//This setup needs to match the VertexType structure in the ModelClass and in the shader
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].InputSlot = 0;
	inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	//Get a count of the elements in the layout
	numElements = sizeof(inputElementDesc) / sizeof(inputElementDesc[0]);

	//Create the vertex input layout
	result = device->CreateInputLayout(inputElementDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//Release the vertex shader buffer and pixel shader buffer since they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from withing this class
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &this->m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Create a texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &this->m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureShader::ShutdownShader()
{

	//Release the sampler state
	if (this->m_sampleState)
	{
		this->m_sampleState->Release();
		this->m_sampleState = nullptr;
	}

	// Release the matrix constant buffer.
	if (this->m_matrixBuffer)
	{
		this->m_matrixBuffer->Release();
		this->m_matrixBuffer = nullptr;
	}

	// Release the layout.
	if (this->m_layout)
	{
		this->m_layout->Release();
		this->m_layout = nullptr;
	}

	// Release the pixel shader.
	if (this->m_pixelShader)
	{
		this->m_pixelShader->Release();
		this->m_pixelShader = nullptr;
	}

	// Release the vertex shader.
	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = nullptr;
	}
}

void TextureShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErrors;
	ofstream fout;

	//Get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//Get the length of the message
	ULONG bufferSize = errorMessage->GetBufferSize();

	//Open a file to write the error message in
	fout.open("shader-error.txt");

	//Write out the error message
	for (ULONG i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	//Close the file
	fout.close();

	//Release the errorMessage
	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFileName, MB_OK);
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(this->m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//Copy the matrices into the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//Unlock the constant buffer
	deviceContext->Unmap(this->m_matrixBuffer, 0);

	//Set the position of the constant buffer in the vertex shader
	bufferNumber = 0;

	//Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->m_matrixBuffer);

	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(this->m_layout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(this->m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->m_pixelShader, nullptr, 0);

	//Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &this->m_sampleState);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);
}