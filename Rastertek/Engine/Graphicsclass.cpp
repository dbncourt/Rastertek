////////////////////////////////////////////////////////////////////////////////
// Filename: Graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	this->m_D3D = NULL;
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
	return true;
}

void GraphicsClass::Shutdown()
{
	if (this->m_D3D)
	{
		this->m_D3D->Shutdown();
		delete this->m_D3D;
		this->m_D3D = NULL;
	}
	return;
}

bool GraphicsClass::Frame()
{
	//Render the graphics scene
	return this->Render();
}

bool GraphicsClass::Render()
{
	//Clear the buffers to begin the scene
	this->m_D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	//Present the rendered scene to the screen
	this->m_D3D->EndScene();

	return true;
}