////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_TextureShader = nullptr;
	this->m_FloorModel = nullptr;
	this->m_BillboardModel = nullptr;
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

	// Create the TextureShader object.
	this->m_TextureShader = new TextureShader();
	if (!this->m_TextureShader)
	{
		return false;
	}

	// Initialize the TextureShader object.
	result = this->m_TextureShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShader object.", L"Error", MB_OK);
		return false;
	}

	//Create the FloorModel object
	this->m_FloorModel = new Model();
	if (!this->m_FloorModel)
	{
		return false;
	}

	//Initialize the FloorModel object
	result = this->m_FloorModel->Initialize(this->m_Direct3D->GetDevice(), "floor.txt", L"grid01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the FloorModel object.", L"Error", MB_OK);
		return false;
	}

	//Create the BillboardModel  object
	this->m_BillboardModel = new Model();
	if (!this->m_BillboardModel)
	{
		return false;
	}

	//Initialize the BillboardModel object
	result = this->m_BillboardModel->Initialize(this->m_Direct3D->GetDevice(), "square.txt", L"seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the BillboardModel object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Graphics::Shutdown()
{
	// Release the FloorModel object.
	if (this->m_FloorModel)
	{
		this->m_FloorModel->Shutdown();
		delete this->m_FloorModel;
		this->m_FloorModel = nullptr;
	}

	// Release the BillboardModel object.
	if (this->m_BillboardModel)
	{
		this->m_BillboardModel->Shutdown();
		delete this->m_BillboardModel;
		this->m_BillboardModel = nullptr;
	}

	//Release the TextureShader object.
	if (this->m_TextureShader)
	{
		this->m_TextureShader->Shutdown();
		delete this->m_TextureShader;
		this->m_TextureShader = nullptr;
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

bool Graphics::Frame(D3DXVECTOR3 position)
{
	bool result;

	//Set the position of the Camera.
	this->m_Camera->SetPosition(position);

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
	D3DXMATRIX translateMatrix;
	
	// Clear the buffers to begin the scene
	this->m_Direct3D->BeginScene(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	// Generate the world, view and projection matrices from the camera and Direct3D objects.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the square FloorModel vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_FloorModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the Model using the FireShader object.
	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_FloorModel->GetTexture());
	if (!result)
	{
		return false;
	}

	// Get the position of the camera.
	D3DXVECTOR3 cameraPosition = this->m_Camera->GetPosition();

	// Set the position of the billboard model.
	D3DXVECTOR3 modelPosition = D3DXVECTOR3(0.0f, 1.5f, 0.0f);

	// Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function
	double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0f / D3DX_PI);

	// Convert rotation into radians
	float rotation = (float)angle * 0.0174532925f;

	// Setup the rotation the billboard at the origin using the world matrix
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Setup the translation matrix from the billboard model
	D3DXMatrixTranslation(&translateMatrix, modelPosition.x, modelPosition.y, modelPosition.z);

	// Finally combine the rotation and translation matrices to create the final world matrix for the billboard model.
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translateMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_BillboardModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader.
	result = this->m_TextureShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_BillboardModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_BillboardModel->GetTexture());
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	this->m_Direct3D->EndScene();

	return true;
}