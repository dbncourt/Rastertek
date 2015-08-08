////////////////////////////////////////////////////////////////////////////////
// Filename: System.cpp
////////////////////////////////////////////////////////////////////////////////
#include "System.h"


System::System()
{
	this->m_Input = nullptr;
	this->m_Graphics = nullptr;
	this->m_Sound = nullptr;
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

	//Create the input object. This object will be used to handle reading the keyboard input from the user
	this->m_Input = new Input();
	if (!this->m_Input)
	{
		return false;
	}

	//Initialize the input object
	result = this->m_Input->Initialize(this->m_hinstance, this->m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(this->m_hwnd, L"Could not initialize the Input object", L"Error", MB_OK);
		return false;
	}

	//Create the graphics object. This object will handle rendering all the graphics for this application
	this->m_Graphics = new Graphics();
	if (!this->m_Graphics)
	{
		return false;
	}
	//Initialize the graphics object
	result = this->m_Graphics->Initialize(screenWidth, screenHeight, this->m_hwnd);
	if (!result)
	{
		return false;
	}

	//Create the Sound object
	this->m_Sound = new Sound();
	if (!this->m_Sound)
	{
		return false;
	}

	//Initialize the Sound object
	result = this->m_Sound->Initialize(this->m_hwnd);
	if (!result)
	{
		MessageBox(this->m_hwnd, L"Could not initialize DirectSound object", L"Error", MB_OK);
		return false;
	}
	return true;
}

void System::Shutdown()
{
	//Release the graphics object
	if (this->m_Graphics)
	{
		this->m_Graphics->Shutdown();
		delete this->m_Graphics;
		this->m_Graphics = nullptr;
	}

	//Release the input object
	if (this->m_Input)
	{
		this->m_Input->Shutdown();
		delete this->m_Input;
		this->m_Input = nullptr;
	}

	//Release the sound object
	if (this->m_Sound)
	{
		this->m_Sound->Shutdown();
		delete this->m_Sound;
		this->m_Sound = nullptr;
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
				MessageBox(this->m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
		}

		//Check if the user pressed escape and wants to quit
		if (this->m_Input->IsEscapePressed())
		{
			done = true;
		}
	}
}

bool System::Frame()
{
	bool result;
	int mouseX;
	int mouseY;

	//Do the input frame processing
	result = this->m_Input->Frame();
	if (!result)
	{
		return false;
	}

	//Get the location of the mouse from the input object
	this->m_Input->GetMouseLocation(mouseX, mouseY);

	//Do the frame processing for the graphics object
	result = this->m_Graphics->Frame(mouseX, mouseY);
	if (!result)
	{
		return false;
	}

	//Finally render the graphics to the screen
	result = this->m_Graphics->Render();
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