////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Model1 = nullptr;
	this->m_Model2 = nullptr;
	this->m_TextureShader = nullptr;
	this->m_TransparentShader = nullptr;

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

	//Create the Model1 object
	this->m_Model1 = new Model();
	if (!this->m_Model1)
	{
		return false;
	}

	//Initialize the Model1 object
	result = this->m_Model1->Initialize(this->m_Direct3D->GetDevice(), "square.txt", L"dirt01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model1 object.", L"Error", MB_OK);
		return false;
	}

	//Create the Model2 object
	this->m_Model2 = new Model();
	if (!this->m_Model2)
	{
		return false;
	}

	//Initialize the Model2 object
	result = this->m_Model2->Initialize(this->m_Direct3D->GetDevice(), "square.txt", L"stone01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model2 object.", L"Error", MB_OK);
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

	//Create the TransparentShader object
	this->m_TransparentShader = new TransparentShader();
	if (!this->m_TransparentShader)
	{
		return false;
	}

	//Initialize the TransparentShader object
	result = this->m_TransparentShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TransparentShader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	//Release the TransparentShader object
	if (this->m_TransparentShader)
	{
		this->m_TransparentShader->Shutdown();
		delete this->m_TransparentShader;
		this->m_TransparentShader = nullptr;
	}

	//Release the TextureShader object
	if (this->m_TextureShader)
	{
		this->m_TextureShader->Shutdown();
		delete this->m_TextureShader;
		this->m_TextureShader = nullptr;
	}

	//Release the Model2 object
	if (this->m_Model2)
	{
		this->m_Model2->Shutdown();
		delete this->m_Model2;
		this->m_Model2 = nullptr;
	}

	//Release the Model1 object
	if (this->m_Model1)
	{
		this->m_Model1->Shutdown();
		delete this->m_Model1;
		this->m_Model1 = nullptr;
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

	// Set the blending amount to 50%.
	float blendAmount = 0.50f;

	//Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	//Get the world, view and projection matrices from the camera and d3d objects
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_Model1->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model with the clip plane shader.
	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model1->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model1->GetTexture());
	if (!result)
	{
		return false;
	}

	// Translate to the right by one unit and towards the camera by one unit.
	D3DXMatrixTranslation(&worldMatrix, 1.0f, 0.0f, -1.0f);

	// Turn on alpha blending for the transparency to work.
	this->m_Direct3D->TurnOnAlphaBlending();

	// Put the second square model on the graphics pipeline.
	this->m_Model2->Render(this->m_Direct3D->GetDeviceContext());

	// Render the second square model with the stone texture and use the 50% blending amount for transparency.
	result = this->m_TransparentShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model2->GetTexture(), blendAmount);
	if (!result)
	{
		return false;
	}

	// Turn off alpha blending.
	this->m_Direct3D->TurnOffAlphaBlending();

	//Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
 }