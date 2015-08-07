////////////////////////////////////////////////////////////////////////////////
// Filename: watershaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "WaterShader.h"


WaterShader::WaterShader()
{
	this->m_vertexShader = 0;
	this->m_pixelShader = 0;
	this->m_inputLayout = 0;
	this->m_samplerState = 0;
	this->m_matrixBuffer = 0;
	this->m_reflectionBuffer = 0;
	this->m_waterBuffer = 0;
}


WaterShader::WaterShader(const WaterShader& other)
{
}


WaterShader::~WaterShader()
{
}


bool WaterShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"WaterVertexShader.hlsl", L"WaterPixelShader.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}


void WaterShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	WaterShader::ShutdownShader();
}


bool WaterShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX reflectionViewMatrix, ID3D11ShaderResourceView* reflectionTexture, ID3D11ShaderResourceView* refractionTexture, ID3D11ShaderResourceView* normalTexture, float waterTranslation, float reflectRefractScale)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = WaterShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix, reflectionTexture, refractionTexture, normalTexture, waterTranslation, reflectRefractScale);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	WaterShader::RenderShader(deviceContext, indexCount);

	return true;
}


bool WaterShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFileName, WCHAR* pixelShaderFileName)
{
	HRESULT result;

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vertexShaderFileName, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vertexShaderFileName);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vertexShaderFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(pixelShaderFileName, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, pixelShaderFileName);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, pixelShaderFileName, L"Missing Shader File", MB_OK);
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

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	ZeroMemory(polygonLayout, sizeof(polygonLayout));

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

	// Get a count of the elements in the layout.
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_inputLayout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	// Create a texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MinLOD = 0;
	samplerDesc.MipLODBias = 0.0f;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &this->m_samplerState);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &this->m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC reflectionBufferDesc;
	ZeroMemory(&reflectionBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// Setup the description of the reflection dynamic constant buffer that is in the vertex shader.
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0;
	reflectionBufferDesc.StructureByteStride = 0;
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&reflectionBufferDesc, nullptr, &this->m_reflectionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC waterBufferDesc;
	ZeroMemory(&waterBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// Setup the description of the water dynamic constant buffer that is in the pixel shader.
	waterBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterBufferDesc.ByteWidth = sizeof(WaterBufferType);
	waterBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterBufferDesc.MiscFlags = 0;
	waterBufferDesc.StructureByteStride = 0;
	waterBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&waterBufferDesc, nullptr, &this->m_waterBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void WaterShader::ShutdownShader()
{
	// Release the WaterConstantBuffer.
	if (this->m_waterBuffer)
	{
		this->m_waterBuffer->Release();
		this->m_waterBuffer = nullptr;
	}

	// Release the ReflectionConstantBuffer.
	if (this->m_reflectionBuffer)
	{
		this->m_reflectionBuffer->Release();
		this->m_reflectionBuffer = nullptr;
	}

	// Release the MatrixConstantBuffer.
	if (this->m_matrixBuffer)
	{
		this->m_matrixBuffer->Release();
		this->m_matrixBuffer = nullptr;
	}

	// Release the SamplerState.
	if (this->m_samplerState)
	{
		this->m_samplerState->Release();
		this->m_samplerState = nullptr;
	}

	// Release the InputLayout
	if (this->m_inputLayout)
	{
		this->m_inputLayout->Release();
		this->m_inputLayout = nullptr;
	}

	// Release the PixelShader.
	if (this->m_pixelShader)
	{
		this->m_pixelShader->Release();
		this->m_pixelShader = nullptr;
	}

	// Release the VertexShader.
	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = nullptr;
	}
}


void WaterShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
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


bool WaterShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX reflectionViewMatrix, ID3D11ShaderResourceView* reflectionTexture, ID3D11ShaderResourceView* refractionTexture, ID3D11ShaderResourceView* normalTexture, float waterTranslation, float reflectRefractScale)
{
	HRESULT result;

	// Transpose all the input matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	D3DXMatrixTranspose(&reflectionViewMatrix, &reflectionViewMatrix);

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
	matrixDataPtr->world = worldMatrix;
	matrixDataPtr->view = viewMatrix;
	matrixDataPtr->projection = projectionMatrix;

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(this->m_matrixBuffer, 0);

	ReflectionBufferType* reflectionDataPtr;

	// Lock the reflection constant buffer so it can be written to.
	result = deviceContext->Map(this->m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	reflectionDataPtr = (ReflectionBufferType*)mappedSubresource.pData;

	// Copy the reflection matrix into the constant buffer.
	reflectionDataPtr->reflection = reflectionViewMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(this->m_reflectionBuffer, 0);

	WaterBufferType*  waterDataPtr;

	// Lock the water constant buffer so it can be written to.
	result = deviceContext->Map(this->m_waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	waterDataPtr = (WaterBufferType*)mappedSubresource.pData;

	// Copy the water data into the constant buffer.
	waterDataPtr->waterTranslation = waterTranslation;
	waterDataPtr->reflectRefractScale = reflectRefractScale;
	waterDataPtr->padding = D3DXVECTOR2(0.0f, 0.0f);

	// Unlock the constant buffer.
	deviceContext->Unmap(this->m_waterBuffer, 0);

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(0, 1, &this->m_matrixBuffer);

	// Finally set the reflection constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(1, 1, &this->m_reflectionBuffer);

	// Finally set the water constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(0, 1, &this->m_waterBuffer);

	// Set the reflection texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &reflectionTexture);

	// Set the refraction texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(1, 1, &refractionTexture);

	// Set the normal map texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(2, 1, &normalTexture);

	return true;
}


void WaterShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(this->m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(this->m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->m_pixelShader, nullptr, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &this->m_samplerState);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}