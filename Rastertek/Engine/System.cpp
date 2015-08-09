////////////////////////////////////////////////////////////////////////////////
// Filename: System.cpp
////////////////////////////////////////////////////////////////////////////////
#include "System.h"


System::System()
{
	this->m_Input = nullptr;
	this->m_Graphics = nullptr;
	this->m_Timer = nullptr;
	this->m_Position = nullptr;
}

System::System(const System& other)
{
}

System::~System()
{
}

bool System::Initialize()
{
	bool result;

	int screenWidth;
	int screenHeight;

	//Initialize the width and height of the screen to zero before sending the variables into the function
	screenWidth = 0;
	screenHeight = 0;

	//Initialize the windows API
	System::InitializeWindows(screenWidth, screenHeight);

	//Create the Input object. This object will be used to handle reading the keyboard input from the user
	this->m_Input = new Input();
	if (!this->m_Input)
	{
		return false;
	}

	//Initialize the Input object
	result = this->m_Input->Initialize(this->m_hinstance, this->m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(this->m_hwnd, L"Could not initialize the Input object", L"Error", MB_OK);
		return false;
	}

	//Create the Graphics object. This object will handle rendering all the graphics for this application
	this->m_Graphics = new Graphics();
	if (!this->m_Graphics)
	{
		return false;
	}
	
	//Initialize the Graphics object
	result = this->m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		MessageBox(this->m_hwnd, L"Could not initialize the Graphics object", L"Error", MB_OK);
		return false;
	}

	// Create the Timer object.
	this->m_Timer = new Timer();
	if (!this->m_Timer)
	{
		return false;
	}

	// Initialize the Timer object.
	result = this->m_Timer->Intialize();
	if (!result)
	{
		MessageBox(this->m_hwnd, L"Could not initialize the Timer object", L"Error", MB_OK);
		return false;
	}

	// Create the Position object.
	this->m_Position = new Position();
	if (!this->m_Position)
	{
		return false;
	}

	// Initialize the Position of the viewer.
	this->m_Position->SetPosition(D3DXVECTOR3(0.0f, 1.5f, -11.0f));

	return true;
}

void System::Shutdown()
{
	// Release the Position object.
	if (this->m_Position)
	{
		delete this->m_Position;
		this->m_Position = nullptr;
	}

	// Release the Timer object.
	if (this->m_Timer)
	{
		delete this->m_Timer;
		this->m_Timer = nullptr;
	}

	//Release the Graphics object
	if (this->m_Graphics)
	{
		this->m_Graphics->Shutdown();
		delete this->m_Graphics;
		this->m_Graphics = nullptr;
	}

	//Release the Input object
	if (this->m_Input)
	{
		this->m_Input->Shutdown();
		delete this->m_Input;
		this->m_Input = nullptr;
	}

	// Shutdown the window.
	System::ShutdownWindows();
}

void System::Run()
{
	bool result;

	MSG msg;
	bool done;

	//Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	//Loop until there is a quit message from the window or the user
	done = false;
	while (!done)
	{
		//Handle the windows messages
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//If windows signals to end the application, then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Otherwise, do the frame processing
			result = System::Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

bool System::Frame()
{
	bool result;

	// Update the system stats.
	this->m_Timer->Frame();

	//Do the Input frame processing
	result = this->m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application
	if (this->m_Input->IsEscapePressed())
	{
		return false;
	}

	// Set the frame time for calculating the update position.
	this->m_Position->SetFrameTime(this->m_Timer->GetTime());

	// Handle the Input.
	this->m_Position->MoveLeft(this->m_Input->IsLeftArrowPressed());

	this->m_Position->MoveRight(this->m_Input->IsRightArrowPressed());

	// Get the view point position / rotation.
	D3DXVECTOR3 position;
	this->m_Position->GetPosition(position);
	
	//Do the frame processing for the Graphics object
	result = this->m_Graphics->Frame(position);
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, umsg, wParam, lParam);
}

void System::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//Get an external pointer to this object
	ApplicationHandle = this;

	//Get the instance of this application
	this->m_hinstance = GetModuleHandle(nullptr);

	//Give the application a name
	this->m_applicationName = L"Engine";

	//Setup the windows class with default settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = this->m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//Register the window class
	RegisterClassEx(&wc);

	//Determine the resolution of the client desktop screen
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Setup the screen settings depending on whether it is running in full screen or in windowed mode
	if (FULL_SCREEN)
	{
		//If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Change the display settings to full screen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//Set the position of the window to the top left corner
		posX = posY = 0;
	}
	else
	{
		//If windowed then set it to 800x600 resolution
		screenWidth = 800;
		screenHeight = 600;

		//Place the window in the middle of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//Create the window with the screen settings and get the handle of it
	this->m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, this->m_applicationName, this->m_applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, nullptr, nullptr, this->m_hinstance, nullptr);

	//Bring the window up on the screen and set it as main focus
	ShowWindow(this->m_hwnd, SW_SHOW);
	SetForegroundWindow(this->m_hwnd);
	SetFocus(this->m_hwnd);

	//Hide the mouse cursor
	ShowCursor(false);
}

void System::ShutdownWindows()
{
	//Show the mouse cursor
	ShowCursor(true);

	//Fix the display settings if leaving full screen mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	//Remove the window
	DestroyWindow(this->m_hwnd);
	this->m_hwnd = nullptr;

	//Remove the application instance
	UnregisterClass(this->m_applicationName, this->m_hinstance);
	m_hinstance = nullptr;

	//Release the pointer to this class
	ApplicationHandle = nullptr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wParam, LPARAM lParam)
{
	switch (umessage)
	{
		//Check if the window is being destroyed
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
		//Check if the window is being closed
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
		//All other messages pass to the message handler in the system class
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wParam, lParam);
	}
	}
}