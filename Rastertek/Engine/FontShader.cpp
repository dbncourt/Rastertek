#include "FontShader.h"


FontShader::FontShader()
{
	this->m_vertexShader = nullptr;
	this->m_pixelShader = nullptr;
	this->m_inputLayout = nullptr;
	this->m_constantBuffer = nullptr;
	this->m_pixelBuffer = nullptr;
	this->m_samplerState = nullptr;
}

FontShader::FontShader(const FontShader& other)
{
}


FontShader::~FontShader()
{
}

bool FontShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex and pixel shaders
	result = FontShader::InitializeShader(device, hwnd, L"FontVertexShader.hlsl", L"FontPixelShader.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void FontShader::Shutdown()
{
	//Shutdown the vertex and pixel shaders as well as the related objects
	FontShader::ShutdownShaders();
}

bool FontShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR fontColor)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = FontShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, fontColor);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	FontShader::RenderShader(deviceContext, indexCount);

	return true;
}

bool FontShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	//Declare and initialize the pointers this function will use to null
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	//Compile the vertex shader code
	result = D3DX11CompileFromFile(vsFilename, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			FontShader::OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//Compile the pixel shader code
	result = D3DX11CompileFromFile(psFilename, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			FontShader::OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	ZeroMemory(&polygonLayout, sizeof(D3D11_INPUT_ELEMENT_DESC) * 2);

	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].SemanticName = "POSITION";

	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].SemanticName = "TEXCOORD";

	//Get a count of the element in the layout
	UINT numElements = sizeof(polygonLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	//Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_inputLayout);
	if (FAILED(result))
	{
		return false;
	}

	//Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//Setup the description of the dynamic constant buffer that is in the vertex shader
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the constant buffer pointer so we can access the vertex shader constants buffer from within this class
	result = device->CreateBuffer(&constantBufferDesc, nullptr, &this->m_constantBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Create a texture sampler state description
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MipLODBias = 0.0f;

	//Create the texture sampler
	result = device->CreateSamplerState(&samplerDesc, &this->m_samplerState);
	if (FAILED(result))
	{
		return false;
	}

	//Setup the description pixel constant buffer that is in the pixel shader
	D3D11_BUFFER_DESC pixelBufferDesc;
	ZeroMemory(&pixelBufferDesc, sizeof(D3D11_BUFFER_DESC));

	pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
	pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;
	pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the pixel constant buffer pointer so we can access the pixel shader constant buffer from whiting this class
	result = device->CreateBuffer(&pixelBufferDesc, nullptr, &this->m_pixelBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void FontShader::ShutdownShaders()
{
	//Release the pixel constant buffer
	if (this->m_pixelBuffer)
	{
		this->m_pixelBuffer->Release();
		this->m_pixelBuffer = nullptr;
	}

	//Release the sampler state
	if (this->m_samplerState)
	{
		this->m_samplerState->Release();
		this->m_samplerState = nullptr;
	}

	//Release the constant buffer
	if (this->m_constantBuffer)
	{
		this->m_constantBuffer->Release();
		this->m_constantBuffer = nullptr;
	}

	//Release the layout
	if (this->m_inputLayout)
	{
		this->m_inputLayout->Release();
		this->m_inputLayout = nullptr;
	}

	//Release the pixel shader
	if (this->m_pixelShader)
	{
		this->m_pixelShader->Release();
		this->m_pixelShader = nullptr;
	}

	//Release the vertex shader
	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = nullptr;
	}
}

void FontShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileError;
	UINT bufferSize;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileError = (char*)errorMessage->GetBufferPointer();

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("Shader-Error.txt");

	// Write out the error message.
	for (UINT i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check Shader-Error.txt for message.", shaderFilename, MB_OK);
}

bool FontShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR fontColor)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(this->m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	ConstantBufferType* constantBufferPtr = (ConstantBufferType*)mappedSubresource.pData;

	//Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	
	//Copy the matrices into the constant buffer
	constantBufferPtr->world = worldMatrix;
	constantBufferPtr->view = viewMatrix;
	constantBufferPtr->projection = projectionMatrix;

	//Unlock the constant buffer
	deviceContext->Unmap(this->m_constantBuffer, 0);

	//Lock the pixel constant buffer so it can be written to
	result = deviceContext->Map(this->m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the pixel buffer
	PixelBufferType* pixelBufferPtr = (PixelBufferType*)mappedSubresource.pData;

	//Copy the pixel color into the pixel constant buffer
	pixelBufferPtr->pixelColor = fontColor;

	//Unlock the pixel constant buffer
	deviceContext->Unmap(this->m_pixelBuffer, 0);

	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	//Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(0, 1, &this->m_constantBuffer);

	//Now set the pixel constant buffer in the pixel shader with the updated value
	deviceContext->PSSetConstantBuffers(0, 1, &this->m_pixelBuffer);

	return true;
}

void FontShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(this->m_inputLayout);

	//Set the vertex and pixel shaders that will be used to render the triangles
	deviceContext->VSSetShader(this->m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->m_pixelShader, nullptr, 0);

	//Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &this->m_samplerState);

	//Render the triangles
	deviceContext->DrawIndexed(indexCount, 0, 0);
}