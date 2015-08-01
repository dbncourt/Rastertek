////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_FloorModel = nullptr;
	this->m_TextureShader = nullptr;
	this->m_RenderTexture = nullptr;
	this->m_ReflectionShader = nullptr;
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
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "cube.txt", L"seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object.", L"Error", MB_OK);
		return false;
	}

	//Create the TextureShader object
	this->m_TextureShader = new TextureShader();
	if (!this->m_TextureShader)
	{
		return false;
	}

	//Initialize the TextureShader object
	result = this->m_TextureShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShader object.", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object
	this->m_RenderTexture = new RenderTexture();
	if (!this->m_RenderTexture)
	{
		return false;
	}

	//Initialize the RenderTexture object
	result = this->m_RenderTexture->Initialize(this->m_Direct3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the RenderTexture object.", L"Error", MB_OK);
		return false;
	}

	//Create the FloorModel object
	this->m_FloorModel = new Model();
	if (!this->m_FloorModel)
	{
		return false;
	}

	//Initialize the FloorModel object
	result = this->m_FloorModel->Initialize(this->m_Direct3D->GetDevice(), "floor.txt", L"blue01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the FloorModel object.", L"Error", MB_OK);
		return false;
	}

	//Create the ReflectionShader object
	this->m_ReflectionShader = new ReflectionShader();
	if (!this->m_ReflectionShader)
	{
		return false;
	}

	//Initialize the ReflectionShader object
	result = this->m_ReflectionShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ReflectionShader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//Release the ReflectionShader object
	if (this->m_ReflectionShader)
	{
		this->m_ReflectionShader->Shutdown();
		delete this->m_ReflectionShader;
		this->m_ReflectionShader = nullptr;
	}

	//Release the FloorModel object
	if (this->m_FloorModel)
	{
		this->m_FloorModel->Shutdown();
		delete this->m_FloorModel;
		this->m_FloorModel = nullptr;
	}

	//Release the RenderTexture object
	if (this->m_RenderTexture)
	{
		this->m_RenderTexture->Shutdown();
		delete this->m_RenderTexture;
		this->m_RenderTexture = nullptr;
	}

	//Release the TextureShader object
	if (this->m_TextureShader)
	{
		this->m_TextureShader->Shutdown();
		delete this->m_TextureShader;
		this->m_TextureShader = nullptr;
	}

	//Release the Model object
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
	//Set the position of the Camera
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));

	return true;
}

bool Graphics::Render()
{
	bool result;

	//Render the entire scene as a reflection to the texture first
	result = Graphics::RenderToTexture();
	if (!result)
	{
		return false;
	}

	// Render the scene as normal to the back buffer
	result = Graphics::RenderScene();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::RenderToTexture()
{
	D3DXMATRIX reflectionViewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;

	static float rotation = 0.0f;

	// Set the render target to be render to texture.
	this->m_RenderTexture->SetRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView());

	// Clear the render to texture
	this->m_RenderTexture->ClearRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix
	this->m_Camera->RenderReflection(-1.5f);

	// Get the camera reflection view matrix instead of the normal view matrix
	this->m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	// Get the world and projection matrices
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader and the reflection view matrix
	this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, this->m_Model->GetTexture());

	// Reset the render target back to the original back buffer and not the render to texture anymore
	this->m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderScene()
{

	bool result;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX reflectionMatrix;

	static float rotation = 0.0f;

	// Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the viewMatrix based on the camera's position
	this->m_Camera->Render();

	// Get the world, view and projection matrices from the camera and Direct3D object
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Multiply the world matrix by the rotation.
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model with the texture shader
	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	// Get the world matrix again and translate down for the floor model to render underneath the cube
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	D3DXMatrixTranslation(&worldMatrix, 0.0f, -1.5f, 0.0f);

	// Get the camera reflection view matrix
	this->m_Camera->GetReflectionViewMatrix(reflectionMatrix);

	// Put the floor model vertex and index buffers on the graphics pipeline to prepare them for drawing
	this->m_FloorModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the floor model using the reflection shader, reflection texture and reflection view matrix.
	result = this->m_ReflectionShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_FloorModel->GetTexture(), this->m_RenderTexture->GetShaderResourceView(), reflectionMatrix);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
}