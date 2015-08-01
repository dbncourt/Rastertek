////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_TranslateShader = nullptr;

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
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "triangle.txt", L"seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object.", L"Error", MB_OK);
		return false;
	}

	//Create the TranslateShader object
	this->m_TranslateShader = new TranslateShader();
	if (!this->m_TranslateShader)
	{
		return false;
	}

	//Initialize the TranslateShader object
	result = this->m_TranslateShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TranslateShader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//Release the TranslateShader object
	if (this->m_TranslateShader)
	{
		this->m_TranslateShader->Shutdown();
		delete this->m_TranslateShader;
		this->m_TranslateShader = nullptr;
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
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));

	return true;
}

bool Graphics::Render()
{
	bool result;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX orthoMatrix;

	static float textureTranslation = 0.0f;

	// Increment the texture translation position
	textureTranslation += 0.01f;
	if (textureTranslation > 1.0f)
	{
		textureTranslation -= 1.0f;
	}

	//Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	//Get the world, view and projection matrices from the camera and d3d objects
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model with the clip plane shader.
	result = this->m_TranslateShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture(), textureTranslation);
	if (!result)
	{
		return false;
	}

	//Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
 }