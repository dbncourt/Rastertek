////////////////////////////////////////////////////////////////////////////////
// Filename: LightShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "LightShader.h"


LightShader::LightShader()
{
	this->m_vertexShader = nullptr;
	this->m_pixelShader = nullptr;
	this->m_inputLayout = nullptr;
	this->m_samplerState = nullptr;
	this->m_matrixBuffer = nullptr;
	this->m_lightColorBuffer = nullptr;
	this->m_lightPositionBuffer = nullptr;
}

LightShader::LightShader(const LightShader& other)
{
}

LightShader::~LightShader()
{
}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex and pixel shaders
	result = LightShader::InitializeShader(device, hwnd, L"LightVertexShader.hlsl", L"LightPixelShader.hlsl");
	if (!result)
	{
		return false;
	}
	return true;
}

void LightShader::Shutdown()
{
	//Shutdown the vertex and pixel shaders as well as the related objects
	LightShader::ShutdownShader();
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR diffuseColor[], D3DXVECTOR4 lightPosition[])
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = LightShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, diffuseColor, lightPosition);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	LightShader::RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFileName, WCHAR* pixelShaderFileName)
{
	HRESULT result;

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	//Compile the vertex shader code.
	result = D3DX11CompileFromFile(vertexShaderFileName, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		//If the shader failed to compile, it should have written something to the error message
		if (errorMessage)
		{
			LightShader::OutputShaderErrorMessage(errorMessage, hwnd, vertexShaderFileName);
		}
		//If there was nothing in the error message then it simply could not find the shader file itself
		else
		{
			MessageBox(hwnd, vertexShaderFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//Compile the pixel shader code
	result = D3DX11CompileFromFile(pixelShaderFileName, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			LightShader::OutputShaderErrorMessage(errorMessage, hwnd, pixelShaderFileName);
		}
		//If there was nothing in the error message then it simply could not find the file itself
		else
		{
			MessageBox(hwnd, pixelShaderFileName, L"Missing Shader File", MB_OK);
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

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	ZeroMemory(polygonLayout, sizeof(polygonLayout));

	//Now setup the layout of the data that goes into the shader
	//This setup needs to match the VertexType structure in the ModelClass and in the shader
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

	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].SemanticName = "NORMAL";

	// Get a count of the elements in the layout.
	UINT numElements = sizeof(polygonLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	//Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_inputLayout);
	if (FAILED(result))
	{
		return false;
	}

	//Release the vertex shader buffer and pixel shader buffer since they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the dynamic Matrix Constant Buffer that is in the vertex shader
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &this->m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightColorBufferDesc;
	ZeroMemory(&lightColorBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the dynamic LightColor Constant Buffer that is in the pixel shader
	lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightColorBufferDesc.ByteWidth = sizeof(LightColorBufferType);
	lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightColorBufferDesc.MiscFlags = 0;
	lightColorBufferDesc.StructureByteStride = 0;
	lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	result = device->CreateBuffer(&lightColorBufferDesc, nullptr, &this->m_lightColorBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightPositionBufferDesc;
	ZeroMemory(&lightPositionBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the dynamic LightPosition Constant Buffer that is in the pixel shader
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;
	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	result = device->CreateBuffer(&lightPositionBufferDesc, nullptr, &this->m_lightPositionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	//Create a texture sampler state description
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

	//Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &this->m_samplerState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void LightShader::ShutdownShader()
{
	//Release the SamplerState
	if (this->m_samplerState)
	{
		this->m_samplerState->Release();
		this->m_samplerState = nullptr;
	}

	//Release the LightColor Constant Buffer
	if (this->m_lightColorBuffer)
	{
		this->m_lightColorBuffer->Release();
		this->m_lightColorBuffer = nullptr;
	}

	//Release the LightPosition Constant Buffer
	if (this->m_lightPositionBuffer)
	{
		this->m_lightPositionBuffer->Release();
		this->m_lightPositionBuffer = nullptr;
	}

	//Release the Matrix Constant Buffer
	if (this->m_matrixBuffer)
	{
		this->m_matrixBuffer->Release();
		this->m_matrixBuffer = nullptr;
	}

	//Release the InputLayout
	if (this->m_inputLayout)
	{
		this->m_inputLayout->Release();
		this->m_inputLayout = nullptr;
	}

	//Release the PixelShader
	if (this->m_pixelShader)
	{
		this->m_pixelShader->Release();
		this->m_pixelShader = nullptr;
	}

	//Release the VertexShader
	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = nullptr;
	}
}

void LightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErrors;
	ofstream fOut;

	//Get a pointer to the error message text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//Get the length of the message
	ULONG bufferSize = errorMessage->GetBufferSize();

	//Open a file to write the error message in
	fOut.open("shader-error.txt");

	//Write out the error message
	for (ULONG i = 0; i < bufferSize; i++)
	{
		fOut << compileErrors[i];
	}

	//Close the file
	fOut.close();

	//Release the errorMessage
	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFileName, MB_OK);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXCOLOR diffuseColor[], D3DXVECTOR4 lightPosition[])
{
	HRESULT result;

	//Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	MatrixBufferType* matrixDataPtr;

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(this->m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	matrixDataPtr = (MatrixBufferType*)mappedSubresource.pData;

	//Copy the matrices into the constant buffer
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->projection = projectionMatrix;

	//Unlock the constant buffer
	deviceContext->Unmap(this->m_matrixBuffer, 0);

	LightColorBufferType* lightColorDataPtr;

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(this->m_lightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	lightColorDataPtr = (LightColorBufferType*)mappedSubresource.pData;

	//Copy the matrices into the constant buffer
	lightColorDataPtr->diffuseColor[0] = diffuseColor[0];
	lightColorDataPtr->diffuseColor[1] = diffuseColor[1];
	lightColorDataPtr->diffuseColor[2] = diffuseColor[2];
	lightColorDataPtr->diffuseColor[3] = diffuseColor[3];

	//Unlock the constant buffer
	deviceContext->Unmap(this->m_lightColorBuffer, 0);

	LightPositionBufferType* lightPositionDataPtr;

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(this->m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	lightPositionDataPtr = (LightPositionBufferType*)mappedSubresource.pData;

	//Copy the matrices into the constant buffer
	lightPositionDataPtr->lightPosition[0] = lightPosition[0];
	lightPositionDataPtr->lightPosition[1] = lightPosition[1];
	lightPositionDataPtr->lightPosition[2] = lightPosition[2];
	lightPositionDataPtr->lightPosition[3] = lightPosition[3];

	//Unlock the constant buffer
	deviceContext->Unmap(this->m_lightPositionBuffer, 0);

	//Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(0, 1, &this->m_matrixBuffer);

	//Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(1, 1, &this->m_lightPositionBuffer);

	//Now set the constant buffer in the pixel shader with the updated values
	deviceContext->PSSetConstantBuffers(0, 1, &this->m_lightColorBuffer);

	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the vertex input layout
	deviceContext->IASetInputLayout(this->m_inputLayout);

	//Set the vertex and pixel shaders that will be used to render this model
	deviceContext->VSSetShader(this->m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->m_pixelShader, nullptr, 0);

	//Set the sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &this->m_samplerState);

	//Render the model
	deviceContext->DrawIndexed(indexCount, 0, 0);
}