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
	this->m_Light1 = nullptr;
	this->m_Light2 = nullptr;
	this->m_Light3 = nullptr;
	this->m_Light4 = nullptr;
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

	//Create the Camera object
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
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "plane01.txt", L"stone01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object.", L"Error", MB_OK);
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
		MessageBox(hwnd, L"Could not initialize the LightShader object.", L"Error", MB_OK);
		return false;
	}

	//Create the Light1 object
	this->m_Light1 = new Light();
	if (!this->m_Light1)
	{
		return false;
	}

	// Initialize the Light1 object.
	this->m_Light1->SetDiffuseColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	this->m_Light1->SetPosition(D3DXVECTOR4(-3.0f, 1.0f, 3.0f, 1.0f));

	//Create the Light2 object
	this->m_Light2 = new Light();
	if (!this->m_Light2)
	{
		return false;
	}

	// Initialize the Light2 object.
	this->m_Light2->SetDiffuseColor(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	this->m_Light2->SetPosition(D3DXVECTOR4(3.0f, 1.0f, 3.0f, 1.0f));

	//Create the Light3 object
	this->m_Light3 = new Light();
	if (!this->m_Light3)
	{
		return false;
	}

	// Initialize the Light3 object.
	this->m_Light3->SetDiffuseColor(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	this->m_Light3->SetPosition(D3DXVECTOR4(-3.0f, 1.0f, -3.0f, 1.0f));

	//Create the Light4 object
	this->m_Light4 = new Light();
	if (!this->m_Light4)
	{
		return false;
	}

	// Initialize the Light4 object.
	this->m_Light4->SetDiffuseColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	this->m_Light4->SetPosition(D3DXVECTOR4(3.0f, 1.0f, -3.0f, 1.0f));

	return true;
}

void Graphics::Shutdown()
{
	//Release the Light1 object
	if (this->m_Light1)
	{
		delete this->m_Light1;
		this->m_Light1 = nullptr;
	}

	//Release the Light object
	if (this->m_Light2)
	{
		delete this->m_Light2;
		this->m_Light2 = nullptr;
	}

	//Release the Light3 object
	if (this->m_Light3)
	{
		delete this->m_Light3;
		this->m_Light3 = nullptr;
	}

	//Release the Light object
	if (this->m_Light4)
	{
		delete this->m_Light4;
		this->m_Light4 = nullptr;
	}

	//Release the LightShader object
	if (this->m_LightShader)
	{
		this->m_LightShader->Shutdown();
		delete this->m_LightShader;
		this->m_LightShader = nullptr;
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
	bool result;

	//Set the position of the Camera
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 2.0f, -12.0f));

	//Render the scene
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

	D3DXCOLOR diffuseColor[4];
	D3DXVECTOR4 lightPosition[4];

	// Create the diffuse color array from the four light colors.
	diffuseColor[0] = this->m_Light1->GetDiffuseColor();
	diffuseColor[1] = this->m_Light2->GetDiffuseColor();
	diffuseColor[2] = this->m_Light3->GetDiffuseColor();
	diffuseColor[3] = this->m_Light4->GetDiffuseColor();

	// Create the light position array from the four light positions.
	lightPosition[0] = this->m_Light1->GetPosition();
	lightPosition[1] = this->m_Light2->GetPosition();
	lightPosition[2] = this->m_Light3->GetPosition();
	lightPosition[3] = this->m_Light4->GetPosition();

	// Clear the buffers to begin the scene.
	this->m_Direct3D->BeginScene(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model using the light shader and the light arrays.
	result = this->m_LightShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture(), diffuseColor, lightPosition);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	this->m_Direct3D->EndScene();

	return true;
}