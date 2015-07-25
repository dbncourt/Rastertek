////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics()
{
	this->m_Direct3D = nullptr;
	this->m_Camera = nullptr;
	this->m_Text = nullptr;
	this->m_Model = nullptr;
	this->m_LightShader = nullptr;
	this->m_Light = nullptr;
	this->m_ModelList = nullptr;
	this->m_Frustum = nullptr;
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
	
	//Create the text object
	this->m_Text = new Text();
	if (!this->m_Text)
	{
		return false;
	}

	//Initialize the text object
	result = this->m_Text->Initialize(this->m_Direct3D->GetDevice(), this->m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Text object.", L"Error", MB_OK);
		return false;
	}

	//Create the Model object
	this->m_Model = new Model();
	if (!this->m_Model)
	{
		return false;
	}

	//Initialize the Model object
	result = this->m_Model->Initialize(this->m_Direct3D->GetDevice(), "sphere.txt", L"seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Model object", L"Error", MB_OK);
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
		MessageBox(hwnd, L"Could not initialize LightShader the object", L"Error", MB_OK);
		return false;
	}

	//Create the Light object
	this->m_Light = new Light();
	if (!this->m_Light)
	{
		return false;
	}

	//Initialize the Light object
	this->m_Light->SetDirection(D3DXVECTOR3(0.0f, 0.0f, 1.0f));

	//Create the ModelList object
	this->m_ModelList = new ModelList();
	if (!this->m_ModelList)
	{
		return false;
	}

	//Initialize the ModelList object
	result = this->m_ModelList->Initialize(25);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize ModelList the object", L"Error", MB_OK);
		return false;
	}

	//Create the Frustum object
	this->m_Frustum = new Frustum();
	if (!this->m_Frustum)
	{
		return false;
	}
	return true;
}

void Graphics::Shutdown()
{
	//Release the Frustum object
	if (this->m_Frustum)
	{
		delete this->m_Frustum;
		this->m_Frustum = nullptr;
	}

	//Release the ModelList object
	if (this->m_ModelList)
	{
		this->m_ModelList->Shutdown();
		delete this->m_ModelList;
		this->m_ModelList = nullptr;
	}

	//Release the Light Object
	if (this->m_Light)
	{
		delete this->m_Light;
		this->m_Light = nullptr;
	}

	//Release the LightShader object
	if (this->m_LightShader)
	{
		this->m_LightShader->Shutdown();
		delete this->m_LightShader;
		this->m_LightShader = nullptr;
	}

	//Release the model object
	if (this->m_Model)
	{
		this->m_Model->Shutdown();
		delete this->m_Model;
		this->m_Model = nullptr;
	}

	//Release the Text object
	if (this->m_Text)
	{
		this->m_Text->Shutdown();
		delete this->m_Text;
		this->m_Text = nullptr;
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

bool Graphics::Frame(float rotationY)
{
	//Set the position of the Camera
	this->m_Camera->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));

	//Set the rotation of the Camera
	this->m_Camera->SetRotation(D3DXVECTOR3(0.0f, rotationY, 0.0f));

	return true;
}

bool Graphics::Render()
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

	//Construct the Frustum
	this->m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//Get the number of models that will be rendered

	//Initialize the count of models that have been rendered
	int renderCount = 0;

	//Go through all the models and render them only if they can be seen by the camera view
	for (int index = 0; index < this->m_ModelList->GetModelCount(); index++)
	{
		bool renderModel;

		D3DXVECTOR3 position;
		D3DXCOLOR color;

		//Get the position and color of the sphere model at this index
		this->m_ModelList->GetData(index, position, color);

		//Check if the sphere model is in the view frustum
		renderModel = this->m_Frustum->CheckSphere(position, 1.0f);//Set the radius of the sphere to 1.0f since is already known

		//If it can be seen then render it, else skip this model and check the next sphere
		if (renderModel)
		{
			//Move the model to the location it should be rendered at
			D3DXMatrixTranslation(&worldMatrix, position.x, position.y, position.z);

			//Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
			this->m_Model->Render(this->m_Direct3D->GetDeviceContext());

			//Render the model using the LightShader
			this->m_LightShader->Render(this->m_Direct3D->GetDeviceContext(), this->m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, this->m_Model->GetTexture(), this->m_Light->GetDirection(), color);

			//Reset to the original world matrix
			this->m_Direct3D->GetWorldMatrix(worldMatrix);
			renderCount++;
		}
	}

	//Set the number of models that was actually rendered this frame
	result = this->m_Text->SetRenderCount(renderCount, this->m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	//Turn off the Z-Buffer to begin all 2D rendering
	this->m_Direct3D->TurnZBufferOff();

	//Turn on the alpha blending before rendering the text
	this->m_Direct3D->TurnOnAlphaBlending();

	//Render the text strings
	result = this->m_Text->Render(this->m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	//Turn off the alpha blending after rendering the text
	this->m_Direct3D->TurnOffAlphaBlending();

	//Turn the Z-Buffer back on now that all 2D rendering has completed
	this->m_Direct3D->TurnZBufferOn();

	//Present the rendered scene to the screen
	this->m_Direct3D->EndScene();

	return true;
}