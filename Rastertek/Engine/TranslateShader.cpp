////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TranslateShader.h"

TranslateShader::TranslateShader()
{
	this->m_vertexShader = nullptr;
	this->m_pixelShader = nullptr;
	this->m_layout = nullptr;
	this->m_sampleState = nullptr;
	this->m_matrixBuffer = nullptr;
	this->m_translationBuffer = nullptr;
}

TranslateShader::TranslateShader(const TranslateShader& other)
{
}

TranslateShader::~TranslateShader()
{
}

bool TranslateShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	//Initialize the vertex and pixel shaders
	return TranslateShader::InitializeShader(device, hwnd, L"TranslateVertexShader.hlsl", L"TranslatePixelShader.hlsl");
}

void TranslateShader::Shutdown()
{
	//Shutdown the vertex and pixel shaders as well as the related objects
	TranslateShader::ShutdownShader();
}

bool TranslateShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float translation)
{
	bool result;
	//Set the shader parameters that it will use for rendering
	result = TranslateShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, translation);
	if (!result)
	{
		return false;
	}

	//Now render the prepared buffer with the shader
	TranslateShader::RenderShader(deviceContext, indexCount);

	return true;
}

bool TranslateShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	
	//Compile the vertex shader code
	result = D3DX11CompileFromFile(vsFileName, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		//If the shader failed to compile, it should have written something to the error message
		if (errorMessage)
		{
			TranslateShader::OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
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
			TranslateShader::OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
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

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	ZeroMemory(polygonLayout, sizeof(polygonLayout));

	//Create the vertex input layout description
	//This setup needs to match the VertexType structure in the ModelClass and in the shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Get a count of the elements in the layout
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->m_layout);
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

	//Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from withing this class
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &this->m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC translationBufferDesc;
	ZeroMemory(&translationBufferDesc, sizeof(D3D11_BUFFER_DESC));

	//Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	translationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	translationBufferDesc.ByteWidth = sizeof(TranslateBufferType);
	translationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	translationBufferDesc.MiscFlags = 0;
	translationBufferDesc.StructureByteStride = 0;
	translationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//Create the constant buffer pointer so we can access the vertex shader constant buffer from withing this class
	result = device->CreateBuffer(&translationBufferDesc, nullptr, &this->m_translationBuffer);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

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

void TranslateShader::ShutdownShader()
{

	//Release the SamplerState
	if (this->m_sampleState)
	{
		this->m_sampleState->Release();
		this->m_sampleState = nullptr;
	}

	// Release the Translate Constant Buffer.
	if (this->m_translationBuffer)
	{
		this->m_translationBuffer->Release();
		this->m_translationBuffer = nullptr;
	}

	// Release the Matrix Constant Buffer.
	if (this->m_matrixBuffer)
	{
		this->m_matrixBuffer->Release();
		this->m_matrixBuffer = nullptr;
	}

	// Release the InputLayout.
	if (this->m_layout)
	{
		this->m_layout->Release();
		this->m_layout = nullptr;
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

void TranslateShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
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

bool TranslateShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float translation)
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

	TranslateBufferType* translateDataPtr;

	//Lock the constant buffer so it can be written to
	result = deviceContext->Map(this->m_translationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer to the data in the constant buffer
	translateDataPtr = (TranslateBufferType*)mappedSubresource.pData;

	// Copy the translation value into the texture translation constant buffer.
	translateDataPtr->translation = translation;

	//Unlock the constant buffer
	deviceContext->Unmap(this->m_translationBuffer, 0);

	//Now set the constant buffer in the vertex shader with the updated values
	deviceContext->VSSetConstantBuffers(0, 1, &this->m_matrixBuffer);

	//Now set the constant buffer in the vertex shader with the updated values
	deviceContext->PSSetConstantBuffers(0, 1, &this->m_translationBuffer);

	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TranslateShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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