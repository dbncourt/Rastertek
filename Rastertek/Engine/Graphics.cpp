////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model = nullptr;
	this->m_SpecMapShader = nullptr;
	this->m_Light = nullptr;

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
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	this->m_Camera->Render();
	this->m_Camera->GetViewMatrix(baseViewMatrix);
	
	//Create the Model object
	this->m_Model = new Model();
	if (!this->m_Model)
	{
		return false;
	}

	//Initialize the Model object
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "cube.txt", L"stone02.dds", L"bump02.dds", L"spec02.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object", L"Error", MB_OK);
		return false;
	}

	//Create the SpecMapShader object
	this->m_SpecMapShader = new SpecMapShader();
	if (!this->m_SpecMapShader)
	{
		return false;
	}

	//Initialize the SpecMapShader object
	result = this->m_SpecMapShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the SpecMapShader object", L"Error", MB_OK);
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
	this->m_Light->SetSpecularColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	this->m_Light->SetSpecularPower(16.0f);

	return true;
}

void Graphics::Shutdown()
{
	//Release the Light object
	if (this->m_Light)
	{
		delete this->m_Light;
		this->m_Light = nullptr;
	}

	//Release the SpecMapShader object
	if (this->m_SpecMapShader)
	{
		this->m_SpecMapShader->Shutdown();
		delete this->m_SpecMapShader;
		this->m_SpecMapShader = nullptr;
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
	static float rotation = 0.0f;

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

	//Update the rotation variable each frame
	rotation += (float)D3DX_PI * 0.0025f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	//Rotate the world matrix by the rotation value
	D3DXMatrixRotationY(&worldMatrix, rotation);

	//Put the model vertex and index buffers on the graphics pipeline them for drawing
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	//Render the model using the MultiTextureShader
 	this->m_SpecMapShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTextureArray(), this->m_Light->GetDirection(), this->m_Light->GetDiffuseColor(), this->m_Camera->GetPosition(), this->m_Light->GetSpecularColor(), this->m_Light->GetSpecularPower());

	//Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
}