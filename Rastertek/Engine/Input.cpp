////////////////////////////////////////////////////////////////////////////////
// Filename: Input.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Input.h"


Input::Input()
{
	this->m_directInput = nullptr;
	this->m_keyboard = nullptr;
	this->m_mouse = nullptr;
}

Input::Input(const Input& other)
{
}

Input::~Input()
{
}

bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	//Store the screen size which will be used for positioning the mouse cursor
	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;

	//Initialize the location of the mouse on the screen
	this->m_mouseX = 0;
	this->m_mouseY = 0;

	//Initialize the main direct input interface
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&this->m_directInput, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//Initialize the direct input interface for the keyboard
	result = this->m_directInput->CreateDevice(GUID_SysKeyboard, &this->m_keyboard, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//Set the data format. In this case since it is a keyboard we can use the predefined data format
	result = this->m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	//Set the cooperative level of the keyboard to not share with other programs
	result = this->m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	//Now acquire the keyboard
	result = this->m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	//Initialize the direct input interface for the mouse
	result = this->m_directInput->CreateDevice(GUID_SysMouse, &this->m_mouse, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//Set the data format for the mouse using the predefined mouse data format
	result = this->m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	//Set the cooperative level of the mouse to share with other programs
	result = this->m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	//Acquire the mouse
	result = this->m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Input::Shutdown()
{
	//Release the mouse
	if (this->m_mouse)
	{
		this->m_mouse->Unacquire();
		this->m_mouse->Release();
		this->m_mouse = nullptr;
	}

	//Release the keyboard
	if (this->m_keyboard)
	{
		this->m_keyboard->Unacquire();
		this->m_keyboard->Release();
		this->m_keyboard = nullptr;
	}

	//Re;ease the main interface to direct input
	if (this->m_directInput)
	{
		this->m_directInput->Release();
		this->m_directInput = nullptr;
	}
}

bool Input::Frame()
{
	bool result;

	//Read the current state of the keyboard
	result = Input::ReadKeyboard();
	if (!result)
	{
		return false;
	}

	//Read the current state of the mouse
	result = Input::ReadMouse();
	if (!result)
	{
		return false;
	}

	//Process the changes in the mouse and keyboard
	Input::ProcessInput(); 

	return true;
}

bool Input::ReadKeyboard()
{
	HRESULT result;

	//Read the keyboard device
	result = this->m_keyboard->GetDeviceState(sizeof(this->m_keyboardState), (LPVOID)&this->m_keyboardState);
	if (FAILED(result))
	{
		//If the keyboard lost focus or was not acquired then try to get control back
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			this->m_keyboard->Acquire();
		} 
		else
		{
			return false;
		}
	}

	return true;
}

bool Input::ReadMouse()
{
	HRESULT result;

	//Read the mouse device
	result = this->m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&this->m_mouseState);
	if (FAILED(result))
	{
		//If the mouse lost focus or was not acquired then try to get control back
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			this->m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Input::ProcessInput()
{
	//Update the location of the mouse cursor based on the change of the mouse location during the frame
	this->m_mouseX += this->m_mouseState.lX;
	this->m_mouseY += this->m_mouseState.lY;

	//Ensure the mouse location doesn't exceed the screen width or height
	if (this->m_mouseX < 0)
	{
		this->m_mouseX = 0;
	} 
	else if (this->m_mouseX > this->m_screenWidth)
	{
		this->m_mouseX = this->m_screenWidth;
	}

	if (this->m_mouseY < 0)
	{
		this->m_mouseY = 0;
	}
	else if (this->m_mouseY > this->m_screenHeight)
	{
		this->m_mouseY = this->m_screenHeight;
	}
}

bool Input::IsEscapePressed()
{
	//Do a bitwise AND on the keyboard state to check if the escape key is currently being pressed
	if (this->m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}
	return false;
}

void Input::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = this->m_mouseX;
	mouseY = this->m_mouseY;
}