////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_WindowModel = nullptr;
	this->m_RenderTexture = nullptr;
	this->m_TextureShader = nullptr;
	this->m_GlassShader = nullptr;
}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//Create the Direct3D object
	this->m_Direct3D = new Direct3D();
	if (!this->m_Direct3D)
	{
		return false;
	}

	//Initialize the Direct3D object
	result = this->m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	//Create the camera object
	this->m_Camera = new Camera();
	if (!this->m_Camera)
	{
		return false;
	}

	//Create the Model object
	this->m_Model = new Model();
	if (!this->m_Model)
	{
		return false;
	}

	//Initialize the Model object
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "cube.txt", L"seafloor.dds", L"bump03.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object.", L"Error", MB_OK);
		return false;
	}

	//Create the WindowModel object
	this->m_WindowModel = new Model();
	if (!this->m_WindowModel)
	{
		return false;
	}

	//Initialize the WindowModel object
	result = this->m_WindowModel->Initialize(this->m_Direct3D->GetDevice(), "square.txt", L"glass01.dds", L"bump03.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the WindowModel object.", L"Error", MB_OK);
		return false;
	}

	//Create the RenderTexture object
	this->m_RenderTexture = new RenderTexture();
	if (!this->m_RenderTexture)
	{
		return false;
	}

	// Initialize the RenderTexture object.
	result = this->m_RenderTexture->Initialize(this->m_Direct3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the RenderTexture object.", L"Error", MB_OK);
		return false;
	}

	//Create the TextureShader object
	this->m_TextureShader = new TextureShader();
	if (!this->m_TextureShader)
	{
		return false;
	}

	// Initialize the TextureShader object.
	result = this->m_TextureShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShader object.", L"Error", MB_OK);
		return false;
	}

	// Create the glass shader object.
	this->m_GlassShader = new GlassShader();
	if (!this->m_GlassShader)
	{
		return false;
	}

	// Initialize the GlassShader object.
	result = this->m_GlassShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the GlassShader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//Release the GlassShader object.
	if (this->m_GlassShader)
	{
		this->m_GlassShader->Shutdown();
		delete this->m_GlassShader;
		this->m_GlassShader = nullptr;
	}

	// Release the TextureShader object.
	if (this->m_TextureShader)
	{
		this->m_TextureShader->Shutdown();
		delete this->m_TextureShader;
		this->m_TextureShader = nullptr;
	}

	// Release the RenderTexture object.
	if (this->m_RenderTexture)
	{
		this->m_RenderTexture->Shutdown();
		delete this->m_RenderTexture;
		this->m_RenderTexture = nullptr;
	}

	// Release the WindowModel object.
	if (this->m_WindowModel)
	{
		this->m_WindowModel->Shutdown();
		delete this->m_WindowModel;
		this->m_WindowModel = nullptr;
	}

	// Release the Model object.
	if (this->m_Model)
	{
		this->m_Model->Shutdown();
		delete this->m_Model;
		this->m_Model = nullptr;
	}

	//Release the Camera object
	if (this->m_Camera)
	{
		delete this->m_Camera;
		this->m_Camera = nullptr;
	}

	//Release the Direct3D object
	if (this->m_Direct3D)
	{
		this->m_Direct3D->Shutdown();
		delete this->m_Direct3D;
		this->m_Direct3D = nullptr;
	}
}

bool Graphics::Frame()
{
	bool result;

	static float rotation;

	// Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	//Set the position of the Camera
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));

	// Render the scene to texture first.
	result = Graphics::RenderToTexture(rotation);
	if (!result)
	{
		return false;
	}

	//Render the scene.
	result = Graphics::Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::RenderToTexture(float rotation)
{
	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;

	bool result;

	// Set the RenderTarget to be the render to texture.
	this->m_RenderTexture->SetRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView());

	// Clear the render to texture
	this->m_RenderTexture->ClearRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and Direct3D object.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Multiply the world matrix by the rotation
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the cube model using the texture shader.
	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	// Reset the RenderTarget back to the original BackBuffer and not the render to texture anymore.
	this->m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::Render(float rotation)
{
	bool result;

	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	float  refractionScale;

	// Set the refraction scale for the glass shader
	refractionScale = 0.01f;

	// Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// Generate the world, view and projection matrices from the camera and Direct3D objects.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Multiply the world matrix by the rotation
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix
	this->m_Direct3D->GetWorldMatrix(worldMatrix);

	// Translate to back where the window model will be rendered.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 0.0f, -1.5f);

	// Put the window model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_WindowModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the cube model using the TextureShader
	result = this->m_GlassShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_WindowModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_WindowModel->GetTexture(), this->m_WindowModel->GetNormalMap(), this->m_RenderTexture->GetShaderResourceView(), refractionScale);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	this->m_Direct3D->EndScene();

	return true;
}