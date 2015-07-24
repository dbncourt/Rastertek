////////////////////////////////////////////////////////////////////////////////
// Filename: System.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

//////////////
// INCLUDES //
//////////////
#include <windows.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Input.h"
#include "Graphics.h"
#include "Sound.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: System
////////////////////////////////////////////////////////////////////////////////
class System
{

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	Graphics* m_Graphics;
	Sound* m_Sound;

public:
	System();
	System(const System& other);
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:
	bool Frame();
	void InitializeWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindows();

};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

/////////////
// GLOBALS //
/////////////
static System* ApplicationHandle = nullptr;
#endif