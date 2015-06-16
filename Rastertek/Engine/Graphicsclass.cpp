////////////////////////////////////////////////////////////////////////////////
// Filename: GraphicsClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	this->m_D3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_TextureShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	//Create the Direct3D object
	this->m_D3D = new D3DClass;
	if (!this->m_D3D)
	{
		return false;
	}

	//Initialize the Direct3D object
	if (!this->m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create the camera object
	this->m_Camera = new CameraClass;
	if (!this->m_Camera)
	{
		return false;
	}

	//Set the initial position of the camera
	this->m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//Create the model object
	this->m_Model = new ModelClass;
	if (!this->m_Model)
	{
		return false;
	}

	//Initialize the model object
	if (!this->m_Model->Initialize(this->m_D3D->GetDevice(), L"seafloor.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//Create the color shader object
	this->m_TextureShader = new TextureShaderClass;
	if (!this->m_TextureShader)
	{
		return false;
	}

	//Initialize the color shader object
	if (!this->m_TextureShader->Initialize(this->m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	//Release the color shader object
	if (this->m_TextureShader)
	{
		this->m_TextureShader->Shutdown();
		delete this->m_TextureShader;
		this->m_TextureShader = nullptr;
	}

	//Release the model object
	if (this->m_Model)
	{
		this->m_Model->Shutdown();
		delete this->m_Model;
		this->m_Model = nullptr;
	}

	//Release the camera object
	if (this->m_Camera)
	{
		delete this->m_Camera;
		this->m_Camera = nullptr;
	}

	//Release the Direct3D object
	if (this->m_D3D)
	{
		this->m_D3D->Shutdown();
		delete this->m_D3D;
		this->m_D3D = nullptr;
	}
	return;
}

bool GraphicsClass::Frame()
{
	//Render the graphics scene
	return GraphicsClass::Render();
}

bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;

	//Clear the buffers to begin the scene
	this->m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate the view matrix based on the camera's position
	this->m_Camera->Render();

	//Get the world, view and projection matrices from the camera and d3d objects
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_D3D->GetWorldMatrix(worldMatrix);
	this->m_D3D->GetProjectionMatrix(projectionMatrix);

	//Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	this->m_Model->Render(this->m_D3D->GetDeviceContext());

	//Render the model using the color shader
	if (!this->m_TextureShader->Render(this->m_D3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture()))
	{
		return false;
	}

	//Present the rendered scene to the screen
	this->m_D3D->EndScene();

	return true;
}