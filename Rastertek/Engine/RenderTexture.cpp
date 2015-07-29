////////////////////////////////////////////////////////////////////////////////
// Filename: RenderTexture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "RenderTexture.h"


RenderTexture::RenderTexture()
{
	this->m_renderTargetTexture = nullptr;
	this->m_renderTargetView = nullptr;
	this->m_shaderResourceView = nullptr;
}

RenderTexture::RenderTexture(const RenderTexture& other)
{
}


RenderTexture::~RenderTexture()
{
}

bool RenderTexture::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC textureDesc;
	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// Setup the render target texture description.
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = textureWidth;

	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, nullptr, &this->m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(this->m_renderTargetTexture, &renderTargetViewDesc, &this->m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//Create the shader resource view
	result = device->CreateShaderResourceView(this->m_renderTargetTexture, &shaderResourceViewDesc, &this->m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void RenderTexture::Shutdown()
{
	if (this->m_shaderResourceView)
	{
		this->m_shaderResourceView->Release();
		this->m_shaderResourceView = nullptr;
	}

	if (this->m_renderTargetView)
	{
		this->m_renderTargetView->Release();
		this->m_renderTargetView = nullptr;
	}

	if (this->m_renderTargetTexture)
	{
		this->m_renderTargetTexture->Release();
		this->m_renderTargetTexture = nullptr;
	}
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &this->m_renderTargetView, depthStencilView);
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha)
{
	float color[4] = { red, green, blue, alpha };

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(this->m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResouceView()
{
	return this->m_shaderResourceView;
}