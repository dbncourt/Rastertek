////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////

#include "systemclass.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){

	SystemClass* System;

	//Create the system object
	System = new SystemClass();
	if (!System)
	{
		return 0;
	}

	//Initialize and run the system object
	if (System->Initialize())
	{
		System->Run();
	}

	//Shutdown and release the system object
	System->Shutdown();
	delete System;
	System = NULL;
	
	return 0;
}