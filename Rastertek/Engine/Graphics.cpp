////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_TextureShader = nullptr;
	this->m_RenderTexture = nullptr;
	this->m_FadeShader = nullptr;
	this->m_Bitmap = nullptr;
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

	//Create the Bitmap object
	this->m_Bitmap = new Bitmap();
	if (!this->m_Bitmap)
	{
		return false;
	}

	//Initialize the Bitmap object
	result = this->m_Bitmap->Initialize(this->m_Direct3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Bitmap object.", L"Error", MB_OK);
		return false;
	}

	// Set the fade in time to 3000 milliseconds
	this->m_fadeInTime = 3000.0f;

	// Initialize the accumulated time to zero milliseconds
	this->m_accumulatedTime = 0.0f;

	// Initialize the fade percentage to zero at first so the scene is black
	this->m_fadePercentage = 0.0f;

	//Set the fading in effect to not done
	this->m_fadeDone = false;

	//Create the FadeShader object
	this->m_FadeShader = new FadeShader();
	if (!this->m_FadeShader)
	{
		return false;
	}

	//Initialize the FadeShader object
	result = this->m_FadeShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the FadeShader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//Release the FadeShader object
	if (this->m_FadeShader)
	{
		this->m_FadeShader->Shutdown();
		delete this->m_FadeShader;
		this->m_FadeShader = nullptr;
	}

	//Release the Bitmap object
	if (this->m_Bitmap)
	{
		this->m_Bitmap->Shutdown();
		delete this->m_Bitmap;
		this->m_Bitmap = nullptr;
	}

	//Release the ReflectionShader object
	if (this->m_FadeShader)
	{
		this->m_FadeShader->Shutdown();
		delete this->m_FadeShader;
		this->m_FadeShader = nullptr;
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

bool Graphics::Frame(float frameTime)
{
	if (!this->m_fadeDone)
	{

		// Update the accumulated time with the extra frame time addition
		this->m_accumulatedTime += frameTime;

		// While the time goes on increase the fade in amount by the time that is passing each frame
		if (this->m_accumulatedTime < this->m_fadeInTime)
		{
			// Calculate the percentage that the screen should be faded in based on the accumulated time
			this->m_fadePercentage = this->m_accumulatedTime / this->m_fadeInTime;
		}
		else
		{
			//If the fade in time is complete then turn off the fade effect and render the scene normally
			this->m_fadeDone = true;

			// Set the percentage to 100%
			this->m_fadePercentage = 1.0f;
		}
	}
	//Set the position of the Camera
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));

	return true;
}

bool Graphics::Render()
{
	bool result;
	static float rotation = 0.0f;

	// Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	
	if (this->m_fadeDone)
	{
		// If fading in is complete then render the scene normally using the back buffer
		Graphics::RenderNormalScene(rotation);
	}
	else
	{
		// If fading in is not complete then render the scene to a texture and fade that texture in
		result = Graphics::RenderToTexture(rotation);
		if (!result)
		{
			return false;
		}

		result = Graphics::RenderFadingScreen();
		if (!result)
		{
			return false;
		}
	}

	return true;
}

bool Graphics::RenderToTexture(float rotation)
{
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;

	// Set the render target to be render to texture.
	this->m_RenderTexture->SetRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView());

	// Clear the render to texture
	this->m_RenderTexture->ClearRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world and projection matrices
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Multiply the worldMatrix by the rotation
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader and the reflection view matrix
	this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture());

	// Reset the render target back to the original back buffer and not the render to texture anymore
	this->m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderFadingScreen()
{

	bool result;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX orthoMatrix;
	D3DXMATRIX worldMatrix;

	// Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
 
	// Generate the viewMatrix based on the camera's position
	this->m_Camera->Render();

	// Get the world, view and projection matrices from the camera and Direct3D object
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all the 2D rendering
	this->m_Direct3D->TurnZBufferOff();

	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing
	result = this->m_Bitmap->Render(this->m_Direct3D->GetDeviceContext(), 0, 0);
	if (!result)
	{
		return false;
	}

	// Render the bitmap using the FadeShader
	result = this->m_FadeShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, this->m_RenderTexture->GetShaderResourceView(), this->m_fadePercentage);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed
	this->m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
}

bool Graphics::RenderNormalScene(float rotation)
{
	bool result;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;

	// Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the buffers to begin the scene
	this->m_Camera->Render();

	// Get the world, view and projection matrices from the camera and Direct3D object
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Multiply the worldMatrix by the rotation
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model with the texture shader
	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen
	this->m_Direct3D->EndScene();
	
	return true;
}