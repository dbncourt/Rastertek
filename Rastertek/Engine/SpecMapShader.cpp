////////////////////////////////////////////////////////////////////////////////
// Filename: SpecMapShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SpecMapShader.h"


SpecMapShader::SpecMapShader()
{
	this->m_vertexShader = nullptr;
	this->m_pixelShader = nullptr;
	this->m_inputLayout = nullptr;
	this->m_samplerState = nullptr;
	this->m_matrixBuffer = nullptr;
	this->m_lightBuffer = nullptr;
	this->m_cameraBuffer = nullptr;
}

SpecMapShader::SpecMapShader(const SpecMapShader& other)
{
}


SpecMapShader::~SpecMapShader()
{
}

bool SpecMapShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex pixel shaders
	result = SpecMapShader::InitializeShader(device, hwnd, L"SpecMapVertexShader.hlsl", L"SpecMapPixelShader.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void SpecMapShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	SpecMapShader::ShutdownShader();
}

bool SpecMapShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDirection, D3DXCOLOR diffuseColor, D3DXVECTOR3 cameraPosition, D3DXCOLOR specularColor, float specularPower)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SpecMapShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, lightDirection, diffuseColor, cameraPosition, specularColor, specularPower);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	SpecMapShader::RenderShader(deviceContext, indexCount);

	return true;
}

bool SpecMapShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{

	HRESULT result;

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFileName, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			SpecMapShader::OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFileName, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			SpecMapShader::OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		else
		{
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &this->m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &this->m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	ZeroMemory(&polygonLayout, sizeof(polygonLayout));

	// Create the vertex input layout description.
	// This setup needs to match the VertexType structure in the ModelClass and in the shader.
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

	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].SemanticName = "TANGENT";

	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].SemanticName = "BINORMAL";

	//Get a count of the elements in the layout
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

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the matrix dynamic constant buffer that is in the vertex shader
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &this->m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC cameraBufferDesc;
	ZeroMemory(&cameraBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the camera dynamic constant buffer that is in the vertex shader
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&cameraBufferDesc, nullptr, &this->m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the light dynamic constant buffer that is in the pixel shader
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, nullptr, &this->m_lightBuffer);
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

void SpecMapShader::ShutdownShader()
{
	//Release the Camera Constant buffer
	if (this->m_cameraBuffer)
	{
		this->m_cameraBuffer->Release();
		this->m_cameraBuffer = nullptr;
	}

	//Release the Light Constant buffer
	if (this->m_lightBuffer)
	{
		this->m_lightBuffer->Release();
		this->m_lightBuffer = nullptr;
	}

	//Release the MatrixConstant buffer
	if (this->m_matrixBuffer)
	{
		this->m_matrixBuffer->Release();
		this->m_matrixBuffer = nullptr;
	}

	//Release the SamplerState
	if (this->m_samplerState)
	{
		this->m_samplerState->Release();
		this->m_samplerState = nullptr;
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

void SpecMapShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
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

bool SpecMapShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDirection, D3DXCOLOR diffuseColor, D3DXVECTOR3 cameraPosition, D3DXCOLOR specularColor, float specularPower)
{
	HRESULT result;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	MatrixBufferType* matrixDataPtr;

	// Lock the matrix constant buffer so it can be written to.
	result = deviceContext->Map(this->m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	matrixDataPtr = (MatrixBufferType*)mappedSubresource.pData;

	// Copy the matrices into the constant buffer.
	matrixDataPtr->worldMatrix = worldMatrix;
	matrixDataPtr->viewMatrix = viewMatrix;
	matrixDataPtr->projectionMatrix = projectionMatrix;

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(this->m_matrixBuffer, 0);

	CameraBufferType* cameraDataPtr;
	
	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(this->m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	cameraDataPtr = (CameraBufferType*)mappedSubresource.pData;

	// Copy the lighting variables into the constant buffer.
	cameraDataPtr->cameraPosition = cameraPosition;
	cameraDataPtr->padding = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(this->m_cameraBuffer, 0);

	LightBufferType* lightDataPtr;

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(this->m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	lightDataPtr = (LightBufferType*)mappedSubresource.pData;

	// Copy the lighting variables into the constant buffer.
	lightDataPtr->diffuseColor = diffuseColor; 
	lightDataPtr->lightDirection = lightDirection;
	lightDataPtr->specularColor = specularColor;
	lightDataPtr->specularPower = specularPower;

	// Unlock the constant buffer.
	deviceContext->Unmap(this->m_lightBuffer, 0);

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(0, 1, &this->m_matrixBuffer);

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(1, 1, &this->m_cameraBuffer);

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(0, 1, &this->m_lightBuffer);

	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 3, textureArray);

	return true;
}

void SpecMapShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(this->m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this model.
	deviceContext->VSSetShader(this->m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->m_pixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &this->m_samplerState);

	// Render the model.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
