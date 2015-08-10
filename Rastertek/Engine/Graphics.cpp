////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_DepthShader = nullptr;
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

	// Set the initial position of the camera.
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 2.0f, -10.0f));

	//Create the Model object
	this->m_Model = new Model();
	if (!this->m_Model)
	{
		return false;
	}

	//Initialize the Model object
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "floor.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object.", L"Error", MB_OK);
		return false;
	}

	// Create the DepthShader object.
	this->m_DepthShader = new DepthShader();
	if (!this->m_DepthShader)
	{
		return false;
	}

	// Initialize the DepthShader object.
	result = this->m_DepthShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the DepthShader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//Release the DepthShader object.
	if (this->m_DepthShader)
	{
		this->m_DepthShader->Shutdown();
		delete this->m_DepthShader;
		this->m_DepthShader = nullptr;
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

	// Render the scene.
	result = Graphics::Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render()
{
	bool result;

	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;

	// Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	// Generate the world, view and projection matrices from the camera and Direct3D objects.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the square model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the Model using the FireShader object.
	result = this->m_DepthShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	this->m_Direct3D->EndScene();

	return true;
}