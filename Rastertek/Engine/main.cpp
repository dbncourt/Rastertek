////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////

#include "System.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd){

	System* system;

	//Create the system object
	system = new System();
	if (!system)
	{
		return 0;
	}

	//Initialize and run the system object
	if (system->Initialize())
	{
		system->Run();
	}

	//Shutdown and release the system object
	system->Shutdown();
	delete system;
	system = nullptr;
	
	return 0;
}