////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_GroundModel = nullptr;
	this->m_WallModel = nullptr;
	this->m_BathModel = nullptr;
	this->m_WaterModel = nullptr;
	this->m_Light = nullptr;
	this->m_RefractionTexture = nullptr;
	this->m_ReflectionTexture = nullptr;
	this->m_LightShader = nullptr;
	this->m_RefractionShader = nullptr;
	this->m_WaterShader = nullptr;
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

	//Create the GroundModel object
	this->m_GroundModel = new Model();
	if (!this->m_GroundModel)
	{
		return false;
	}

	//Initialize the GroundModel object
	result = this->m_GroundModel->Initialize(this->m_Direct3D->GetDevice(), "ground.txt", L"ground01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the GroundModel object.", L"Error", MB_OK);
		return false;
	}

	//Create the WallModel object
	this->m_WallModel = new Model();
	if (!this->m_WallModel)
	{
		return false;
	}

	//Initialize the WallModel object
	result = this->m_WallModel->Initialize(this->m_Direct3D->GetDevice(), "wall.txt", L"wall01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the WallModel object.", L"Error", MB_OK);
		return false;
	}

	//Create the BathModel object
	this->m_BathModel = new Model();
	if (!this->m_BathModel)
	{
		return false;
	}

	//Initialize the BathModel object
	result = this->m_BathModel->Initialize(this->m_Direct3D->GetDevice(), "bath.txt", L"marble01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the BathModel object.", L"Error", MB_OK);
		return false;
	}

	//Create the WaterModel object
	this->m_WaterModel = new Model();
	if (!this->m_WaterModel)
	{
		return false;
	}

	//Initialize the WaterModel object
	result = this->m_WaterModel->Initialize(this->m_Direct3D->GetDevice(), "water.txt", L"water01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the WaterModel object.", L"Error", MB_OK);
		return false;
	}

	//Create the Light object
	this->m_Light = new Light();
	if (!this->m_Light)
	{
		return false;
	}

	// Initialize the Light object.
	this->m_Light->SetAmbientColor(D3DXCOLOR(0.15f, 0.15f, 0.15f, 1.0f));
	this->m_Light->SetDiffuseColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	this->m_Light->SetDirection(D3DXVECTOR3(0.0f, -1.0f, 0.5f));

	//Create the Refraction RenderTexture object
	this->m_RefractionTexture = new RenderTexture();
	if (!this->m_RefractionTexture)
	{
		return false;
	}

	// Initialize the Refraction RenderTexture object.
	result = this->m_RefractionTexture->Initialize(this->m_Direct3D->GetDevice(), screenWidth, screenHeight);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the Refraction RenderTexture object.", L"Error", MB_OK);
		return false;
	}

	//Create the Reflection RenderTexture object
	this->m_ReflectionTexture = new RenderTexture();
	if (!this->m_ReflectionTexture)
	{
		return false;
	}

	// Initialize the Reflection RenderTexture object.
	result = this->m_ReflectionTexture->Initialize(this->m_Direct3D->GetDevice(), screenWidth, screenHeight);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the Reflection RenderTexture object.", L"Error", MB_OK);
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

	// Create the RefractionShader object
	this->m_RefractionShader = new RefractionShader();
	if (!this->m_RefractionShader)
	{
		return false;
	}

	//Initialize the RefractionShader object
	result = this->m_RefractionShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the RefractionShader object.", L"Error", MB_OK);
		return false;
	}

	//Create the WaterShader object
	this->m_WaterShader = new WaterShader();
	if (!this->m_WaterShader)
	{
		return false;
	}

	//Initialize the WaterShader object
	result = this->m_WaterShader->Initialize(this->m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the BitmapWaterShader object.", L"Error", MB_OK);
		return false;
	}

	// Set the height of the water.
	this->m_waterHeight = 2.75f;

	// Initialize the position of the water.
	this->m_waterTranslation = 0.0f;
	
	return true;
}

void Graphics::Shutdown()
{
	//Release the WaterShader object
	if (this->m_WaterShader)
	{
		this->m_WaterShader->Shutdown();
		delete this->m_WaterShader;
		this->m_WaterShader = nullptr;
	}

	//Release the RefractionShader object
	if (this->m_RefractionShader)
	{
		this->m_RefractionShader->Shutdown();
		delete this->m_RefractionShader;
		this->m_RefractionShader = nullptr;
	}

	//Release the LightShader object
	if (this->m_LightShader)
	{
		this->m_LightShader->Shutdown();
		delete this->m_LightShader;
		this->m_LightShader = nullptr;
	}

	//Release the Reflection RenderTexture object
	if (this->m_ReflectionTexture)
	{
		this->m_ReflectionTexture->Shutdown();
		delete this->m_ReflectionTexture;
		this->m_ReflectionTexture = nullptr;
	}

	//Release the Refraction RenderTexture object
	if (this->m_RefractionTexture)
	{
		this->m_RefractionTexture->Shutdown();
		delete this->m_RefractionTexture;
		this->m_RefractionTexture = nullptr;
	}

	//Release the Light object
	if (this->m_Light)
	{
		delete this->m_Light;
		this->m_Light = nullptr;
	}

	//Release the WaterModel object
	if (this->m_WaterModel)
	{
		this->m_WaterModel->Shutdown();
		delete this->m_WaterModel;
		this->m_WaterModel = nullptr;
	}

	//Release the BathModel object
	if (this->m_BathModel)
	{
		this->m_BathModel->Shutdown();
		delete this->m_BathModel;
		this->m_BathModel = nullptr;
	}

	//Release the WallModel object
	if (this->m_WallModel)
	{
		this->m_WallModel->Shutdown();
		delete this->m_WallModel;
		this->m_WallModel = nullptr;
	}

	//Release the GroundModel object
	if (this->m_GroundModel)
	{
		this->m_GroundModel->Shutdown();
		delete this->m_GroundModel;
		this->m_GroundModel = nullptr;
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
	// Update the position of the water to simulate motion.
	this->m_waterTranslation += 0.001f;
	if (this->m_waterTranslation > 1.0f)
	{
		this->m_waterTranslation -= 1.0f;
	}

	//Set the position of the Camera
	this->m_Camera->SetPosition(D3DXVECTOR3(-10.0f, 6.0f, -10.0f));
	//Set the rotation of the Camera
	this->m_Camera->SetRotation(D3DXVECTOR3(0.0f, 45.0f, 0.0f));

	return true;
}

bool Graphics::Render()
{
	bool result;
	
	// Render the refraction of the scene to a texture.
	result = Graphics::RenderRefractionToTexture();
	if (!result)
	{
		return false;
	}

	// Render the reflection of the scene to a texture.
	result = Graphics::RenderReflectionToTexture();
	if (!result)
	{
		return false;
	}

	// Render the scene as normal to the back buffer.
	result = Graphics::RenderScene();
	if (!result)
	{
		return false;
	}
	return true;
}

bool Graphics::RenderRefractionToTexture()
{
	bool result;

	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;

	D3DXVECTOR4 clipPlane;

	// Setup a clipping plane based on the height of the water to clip everything above it.
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, this->m_waterHeight + 0.1f);

	// Set the render target to be the Refraction RenderTexture.
	this->m_RefractionTexture->SetRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView());

	// Clear the Refraction RenderTexture.
	this->m_RefractionTexture->ClearRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the ViewMatrix based on the camera's position.
	this->m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the BathModel will be rendered.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 2.0f, 0.0f);

	// Put the BathModel vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_BathModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the BathModel using the LightShader.
	result = this->m_RefractionShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_BathModel->GetTexture(), this->m_Light->GetDirection(), this->m_Light->GetAmbientColor(), this->m_Light->GetDiffuseColor(), clipPlane);
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	this->m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderReflectionToTexture()
{

	bool result;

	D3DXMATRIX worldMatrix;
	D3DXMATRIX reflectionViewMatrix;
	D3DXMATRIX projectionMatrix;

	// Set the render target to be the Reflection RenderTexture.
	this->m_ReflectionTexture->SetRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView());

	// Clear the Reflection RenderTexture.
	this->m_ReflectionTexture->ClearRenderTarget(this->m_Direct3D->GetDeviceContext(), this->m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to render the Reflection and create a ReflectionViewMatrix.
	this->m_Camera->RenderReflection(m_waterHeight);

	// Get the camera ReflectionViewMatrix instead of the normal ViewMatrix.
	this->m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	// Get the world and projection matrices from the d3d object.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the WallModel will be rendered.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 6.0f, 8.0f);

	// Put the WallModel vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_WallModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the WallModel using the LightShader and the ReflectionViewMatrix.
	result = this->m_LightShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_WallModel->GetIndexCount(), worldMatrix, reflectionViewMatrix, projectionMatrix, this->m_WallModel->GetTexture(), this->m_Light->GetDirection(), this->m_Light->GetAmbientColor(), this->m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	this->m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool Graphics::RenderScene()
{
	bool result;

	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX reflectionViewMatrix;
	D3DXMATRIX projectionMatrix;

	// Clear the buffers to begin the scene.
	this->m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	this->m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);
	this->m_Camera->GetViewMatrix(viewMatrix);
	this->m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the GroundModel will be rendered.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 1.0f, 0.0f);

	// Put the GroundModel vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_GroundModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the GroundModel using the LightShader.
	result = this->m_LightShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_GroundModel->GetTexture(), this->m_Light->GetDirection(), this->m_Light->GetAmbientColor(), this->m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the worldMatrix.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);

	// Translate to where the WallModel will be rendered.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 6.0f, 8.0f);

	// Put the WallModel vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_WallModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the WallModel using the LightShader.
	result = this->m_LightShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_WallModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_WallModel->GetTexture(), this->m_Light->GetDirection(), this->m_Light->GetAmbientColor(), this->m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the worldMatrix.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);

	// Translate to where the BathModel will be rendered.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, 2.0f, 0.0f);

	// Put the BathModel vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_BathModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the BathModel using the LightShader.
	result = this->m_LightShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_BathModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_BathModel->GetTexture(), this->m_Light->GetDirection(), this->m_Light->GetAmbientColor(), this->m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the worldMatrix.
	this->m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the camera ReflectionViewMatrix.
	this->m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	// Translate to where the WaterModel will be rendered.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, this->m_waterHeight, 0.0f);

	// Put the WaterModel vertex and index buffers on the graphics pipeline to prepare them for drawing.
	this->m_WaterModel->Render(this->m_Direct3D->GetDeviceContext());

	// Render the WaterModel using the WaterShader.
	result = this->m_WaterShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, reflectionViewMatrix, this->m_ReflectionTexture->GetShaderResourceView(), this->m_RefractionTexture->GetShaderResourceView(), this->m_WaterModel->GetTexture(), this->m_waterTranslation, 0.01f);
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	this->m_Direct3D->EndScene();

	return true;
}