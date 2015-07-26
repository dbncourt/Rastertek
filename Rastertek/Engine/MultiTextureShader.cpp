////////////////////////////////////////////////////////////////////////////////
// Filename: MultiTextureShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "MultiTextureShader.h"


MultiTextureShader::MultiTextureShader()
{
	this->m_vertexShader = nullptr;
	this->m_pixelShader = nullptr;
	this->m_inputLayout = nullptr;
	this->m_matrixBuffer = nullptr;
	this->m_samplerState = nullptr;
}

MultiTextureShader::MultiTextureShader(const MultiTextureShader& other)
{
}


MultiTextureShader::~MultiTextureShader()
{
}

bool MultiTextureShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Initialize the vertex and pixel shaders
	result = MultiTextureShader::InitializeShader(device, hwnd, L"MultiTextureVertexShader.hlsl", L"MultiTexturePixelShader.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void MultiTextureShader::Shutdown()
{
	//Shutdown the vertex and pixel shaders as well as the related objects
	MultiTextureShader::ShutdownShader();
}


bool MultiTextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray)
{
	bool result;

	//Set the shader parameters that it will use for rendering
	result = MultiTextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffers with the shader
	MultiTextureShader::RenderShader(deviceContext, indexCount);

	return true;
}

bool MultiTextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	//Compile the vertex shader code
	result = D3DX11CompileFromFile(vsFileName, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		//If the shader failed to compile it should have written something to the error message
		if (errorMessage)
		{
			MultiTextureShader::OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//Compile the pixel shader code
	result = D3DX11CompileFromFile(psFilename, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			MultiTextureShader::OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
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

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	ZeroMemory(&polygonLayout, sizeof(polygonLayout));
	//Create the vertex input layout description

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

	//Get a count of the elements in the layout
	int numElements = sizeof(polygonLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	//Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_inputLayout);
	if (FAILED(result))
	{
		return false;
	}

	//Release the vertex shader buffer and pixel buffer since they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//Setup the description of the matrix dynamic constant buffer that is in the vertex shader
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));

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

	//Create the texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &this->m_samplerState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void MultiTextureShader::ShutdownShader()
{
	//Release the SamplerState
	if (this->m_samplerState)
	{
		this->m_samplerState->Release();
		this->m_samplerState = nullptr;
	}

	//Release the matrix constant buffer
	if (this->m_matrixBuffer)
	{
		this->m_matrixBuffer->Release();
		this->m_matrixBuffer = nullptr;
	}

	//Release the input layout
	if (this->m_inputLayout)
	{
		this->m_inputLayout->Release();
		this->m_inputLayout = nullptr;
	}

	//Release the pixel shader
	if (this->m_pixelShader)
	{
		this->m_pixelShader->Release();
		this->m_inputLayout = nullptr;
	}

	//Release the vertex shader
	if (this->m_vertexShader)
	{
		this->m_vertexShader->Release();
		this->m_vertexShader = nullptr;
	}
}

void MultiTextureShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileError;
	UINT bufferSize;
	ofstream fout;

	compileError = (char*)errorMessage->GetBufferPointer();
	bufferSize = errorMessage->GetBufferSize();

	fout.open("Shader-Error.txt");
	for (UINT i = 0; i < bufferSize; i++)
	{
		fout << compileError[i];
	}
	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;
}

bool MultiTextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray)
{
	HRESULT result;

	//Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	MatrixBufferType* dataPtr;

	//Lock the matrix constant buffer so it can be written to
	result = deviceContext->Map(this->m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	dataPtr = (MatrixBufferType*)mappedSubresource.pData;

	//Copy the matrices into the constant buffer
	dataPtr->worldMatrix = worldMatrix;
	dataPtr->viewMatrix = viewMatrix;
	dataPtr->projectionMatrix = projectionMatrix;

	//Unlock the matrix constant buffer.
	deviceContext->Unmap(this->m_matrixBuffer, 0);

	//Now set the matrix constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(0, 1, &this->m_matrixBuffer);

	//Set shader texture array resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 2, textureArray);

	return true;
}

void MultiTextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{

	//Set the vertex input layout
	deviceContext->IASetInputLayout(this->m_inputLayout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(this->m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->m_pixelShader, nullptr, 0);

	//Set the sampler state in the pixel
	deviceContext->PSSetSamplers(0, 1, &this->m_samplerState);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);
}