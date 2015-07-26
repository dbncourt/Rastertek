////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_AlphaMapShader = nullptr;

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
	//Set the initial position of the camera
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));
	this->m_Camera->Render();
	this->m_Camera->GetViewMatrix(baseViewMatrix);
	
	//Create the Model object
	this->m_Model = new Model();
	if (!this->m_Model)
	{
		return false;
	}

	//Initialize the Model object
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "square.txt", L"stone01.dds", L"dirt01.dds", L"alpha01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object", L"Error", MB_OK);
		return false;
	}

	//Create the AlphaMapShader object
	this->m_AlphaMapShader = new AlphaMapShader();
	if (!this->m_AlphaMapShader)
	{
		return false;
	}

	//Initialize the AlphaMapShader object
	result = this->m_AlphaMapShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the AlphaMapShader object", L"Error", MB_OK);
		return false;
	}
	return true;
}

void Graphics::Shutdown()
{
	//Release the AlphaMapShader object
	if (this->m_AlphaMapShader)
	{
		this->m_AlphaMapShader->Shutdown();
		delete this->m_AlphaMapShader;
		this->m_AlphaMapShader = nullptr;
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

	//Put the model vertex and index buffers on the graphics pipeline them for drawing
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	//Render the model using the MultiTextureShader
	this->m_AlphaMapShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTextureArray());

	//Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
}