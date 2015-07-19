////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_TexureShader = nullptr;
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
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create the camera object
	this->m_Camera = new Camera();
	if (!this->m_Camera)
	{
		return false;
	}
	//Set the initial position of the camera
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));

	//Create the texture shader object
	this->m_TexureShader = new TextureShader();
	if (!this->m_TexureShader)
	{
		return false;
	}

	result = this->m_TexureShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShader object.", L"Error", MB_OK);
		return false;
	}

	//Create the model object
	this->m_Bitmap = new Bitmap();
	if (!this->m_Bitmap)
	{
		return false;
	}

	//Initialize the model object
	result = this->m_Bitmap->Initialize(this->m_Direct3D->GetDevice(), screenWidth, screenHeight, L"seafloor.dds", 256, 256);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Bitmap object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

void Graphics::Shutdown()
{
	//Release the light shader object
	if (this->m_TexureShader)
	{
		this->m_TexureShader->Shutdown();
		delete this->m_TexureShader;
		this->m_TexureShader = nullptr;
	}

	//Release the model object
	if (this->m_Bitmap)
	{
		this->m_Bitmap->Shutdown();
		delete this->m_Bitmap;
		this->m_Bitmap = nullptr;
	}

	//Release the camera object
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
	static float rotation = 0.0f;

	//Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation = -360.0f;
	}
	//Render the graphics scene
	return Graphics::Render(rotation);
}

bool Graphics::Render(float rotation)
{
	bool result;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX orthoMatrix;

	//Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate the view matrix based on the camera's position
	this->m_Camera->Render();

	//Get the world, view and projection matrices from the camera and d3d objects
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);
	this->m_Direct3D->GetOrthoMatrix(orthoMatrix);

	//Turn off the Z-Buffer to begin all 2D rendering
	this->m_Direct3D->TurnZBufferOff();

	//Put the bitmap vertex and index buffer on the graphics pipeline to prepare them for drawing
	result = this->m_Bitmap->Render(this->m_Direct3D->GetDeviceContext(), 150, 150);
	if (!result)
	{
		return false;
	}

	//Render the bitmap with the texture shader
	result = this->m_TexureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, this->m_Bitmap->GetTexture());
	if (!result)
	{
		return false;
	}

	//Turn the Z-Buffer back on now that all 2D rendering has completed
	this->m_Direct3D->TurnZBufferOn();

	//Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
}