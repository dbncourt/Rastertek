////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_LightShader = nullptr;
	this->m_Light = nullptr;
	this->m_RenderTexture = nullptr;
	this->m_DebugWindow = nullptr;
	this->m_TextureShader = nullptr;

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
	D3DXMATRIX baseViewMatrix;

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
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
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
		MessageBox(hwnd, L"Could not initialize the Model object", L"Error", MB_OK);
		return false;
	}

	//Create the LightShader object
	this->m_LightShader = new LightShader();
	if (!this->m_LightShader)
	{
		return false;
	}

	//Initialize the LightShader object
	result = this->m_LightShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the LightShader object", L"Error", MB_OK);
		return false;
	}

	//Create the Light object
	this->m_Light = new Light();
	if (!this->m_Light)
	{
		return false;
	}

	//Initialize the Light Object
	this->m_Light->SetDiffuseColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	this->m_Light->SetDirection(D3DXVECTOR3(0.0f, 0.0f, 1.0f));

	//Create the RenderTexture object
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

	//Create the DebugWindow object
	this->m_DebugWindow = new DebugWindow();
	if (!this->m_DebugWindow)
	{
		return false;
	}

	//Initialize the DebugWindow object
	result = this->m_DebugWindow->Initialize(this->m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the DebugWindow object.", L"Error", MB_OK);
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

	return true;
}

void Graphics::Shutdown()
{
	//Release the TextureShader object
	if (this->m_TextureShader)
	{
		this->m_TextureShader->Shutdown();
		delete this->m_TextureShader;
		this->m_TextureShader = nullptr;
	}

	//Release the DebugWindow object
	if (this->m_DebugWindow)
	{
		this->m_DebugWindow->Shutdown(); 
		delete this->m_DebugWindow;
		this->m_DebugWindow = nullptr;
	}

	//Release the RenderTexture object
	if (this->m_RenderTexture)
	{
		this->m_RenderTexture->Shutdown();
		delete this->m_RenderTexture;
		this->m_RenderTexture = nullptr;
	}

	//Release the Light object
	if (this->m_Light)
	{
		delete this->m_Light;
		this->m_Light = nullptr;
	}

	//Release the LightShader object
	if (this->m_LightShader)
	{
		this->m_LightShader->Shutdown();
		delete this->m_LightShader;
		this->m_LightShader = nullptr;
	}

	//Release the model object
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
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -5.0f));

	return true;
}

bool Graphics::Render()
{
	bool result;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX orthoMatrix;

	// Render the entire scene to the texture first.
	result = Graphics::RenderToTexture();
	if (!result)
	{
		return false;
	}

	//Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Render the scene as normal to the back buffer.
	result = Graphics::RenderScene();
	if (!result)
	{
		return false;
	}

	//Turn off the Z buffer to begin all 2D rendering
	this->m_Direct3D->TurnZBufferOff();

	//Get the world, view and projection matrices from the camera and d3d objects
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = this->m_DebugWindow->Render(this->m_Direct3D->GetDeviceContext(), 50, 50);
	if (!result)
	{
		return false;
	}

	// Render the debug window using the texture shader.
	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, this->m_RenderTexture->GetShaderResouceView());
	if (!result)
	{
		return false;
	}

	//Turn the Z buffer back on now that all 2D rendering has completed
	this->m_Direct3D->TurnZBufferOn();

	//Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
}

bool Graphics::RenderToTexture()
{
	bool result;

	// Set the render target to be the render to texture.
	this->m_RenderTexture->SetRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView());

	// Clear the render to texture.
	this->m_RenderTexture->ClearRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = Graphics::RenderScene();
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	this->m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderScene()
{
	bool result;

	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;

	static float rotation = 0.0f;


	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Multiply the world matrix by the rotation.
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model with the light shader.
	result = this->m_LightShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture(), this->m_Light->GetDirection(), this->m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	return true;
}
