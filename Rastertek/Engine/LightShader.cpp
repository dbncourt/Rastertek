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
	this->m_cameraBuffer = nullptr;
	this->m_lightBuffer = nullptr;
}


LightShader::~LightShader()
{
}

LightShader::LightShader(const LightShader& other)
{
}

bool LightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = LightShader::InitializeShader(device, hwnd, L"LightVertexShader.hlsl", L"LightPixelShader.hlsl");
	if (!result)
	{
		return false;
	}
	return true;
}

void LightShader::Shutdown()
{
	LightShader::ShutdownShaders();
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXCOLOR ambientColor, D3DXCOLOR diffuseColor, D3DXVECTOR3 cameraPosition, D3DXCOLOR specularColor, float specularPower)
{
	bool result;

	result = LightShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
	if (!result)
	{
		return false;
	}
	LightShader::RenderShader(deviceContext, indexCount);
	return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	result = D3DX11CompileFromFile(vsFilename, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			LightShader::OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			LightShader::OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

    // Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &this->m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &this->m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType structure in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[3];
	ZeroMemory(&inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC) * 3);

	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].SemanticName = "POSITION";

	inputElementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].InputSlot = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].SemanticName = "TEXCOORD";

	inputElementDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].InputSlot = 0;
	inputElementDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[2].InstanceDataStepRate = 0;
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].SemanticName = "NORMAL";

	UINT numElements = sizeof(inputElementDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	result = device->CreateInputLayout(inputElementDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_inputLayout);
	if (FAILED(result))
	{
		return false;
	}
	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr; 

	// Create a texture sampler state description.
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
	
	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &this->m_samplerState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &this->m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Setup the description of the camera dynamic constant buffer that is in the vertex shader
	D3D11_BUFFER_DESC cameraBufferDesc;
	ZeroMemory(&cameraBufferDesc, sizeof(D3D11_BUFFER_DESC));
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	
	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, nullptr, &this->m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, nullptr, &this->m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void LightShader::ShutdownShaders()
{
	if (this->m_cameraBuffer)
	{
		this->m_cameraBuffer->Release();
		this->m_cameraBuffer = nullptr;
	}

	if (this->m_lightBuffer)
	{
		this->m_lightBuffer->Release();
		this->m_lightBuffer = nullptr;
	}

	if (this->m_matrixBuffer)
	{
		this->m_matrixBuffer->Release();
		this->m_matrixBuffer = nullptr;
	}

	if (this->m_samplerState)
	{
		this->m_samplerState->Release();
		this->m_samplerState = nullptr;
	}

	if (this->m_inputLayout)
	{
		this->m_inputLayout->Release();
		this->m_inputLayout = nullptr;
	}

	if (this->m_pixelShader)
	{
		this->m_pixelShader->Release();
		this->m_pixelShader = nullptr;
	}

	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = nullptr;
	}
}

void LightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXCOLOR ambientColor, D3DXCOLOR diffuseColor, D3DXVECTOR3 cameraPosition, D3DXCOLOR specularColor, float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	MatrixBufferType* matrixDataPtr;
	LightBufferType* lightDataPtr;
	CameraBufferType* cameraDataPtr;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(this->m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}
	// Get a pointer to the data in the constant buffer.
	matrixDataPtr = (MatrixBufferType*)mappedSubresource.pData;
	// Copy the matrices into the constant buffer.
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->projection = projectionMatrix;
	// Unlock the constant buffer.
	deviceContext->Unmap(this->m_matrixBuffer, 0);

	// Lock the camera constant buffer so it can be written to.
	result = deviceContext->Map(this->m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}
	// Get a pointer to the data in the constant buffer.
	cameraDataPtr = (CameraBufferType*)mappedSubresource.pData;
	// Copy the camera position into the constant buffer.
	cameraDataPtr->cameraPosition = cameraPosition;
	cameraDataPtr->padding = 0.0f;
	// Unlock the camera constant buffer.
	deviceContext->Unmap(this->m_cameraBuffer, 0);

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(this->m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}
	// Get a pointer to the data in the light constant buffer.
	lightDataPtr = (LightBufferType*)mappedSubresource.pData;

	// Copy the lighting variables into the light constant buffer.
	lightDataPtr->ambientColor = ambientColor;
	lightDataPtr->diffuseColor = diffuseColor;
	lightDataPtr->lightDirection = lightDirection;
	lightDataPtr->specularColor = specularColor;
	lightDataPtr->specularPower = specularPower;
	// Unlock the light constant buffer.
	deviceContext->Unmap(this->m_lightBuffer, 0);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	
	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(0, 1, &this->m_matrixBuffer);
	
	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(1, 1, &this->m_cameraBuffer);
	
	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(0, 1, &this->m_lightBuffer);

	return true; 
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(this->m_inputLayout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(this->m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->m_pixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &this->m_samplerState);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}