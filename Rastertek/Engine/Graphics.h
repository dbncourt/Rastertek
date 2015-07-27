////////////////////////////////////////////////////////////////////////////////
// Filename: Graphics.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Direct3D.h"
#include "Camera.h"
#include "Model.h"
#include "BumpMapShader.h"
#include "Light.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: Graphics
////////////////////////////////////////////////////////////////////////////////
class Graphics
{

private:
	Direct3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	BumpMapShader* m_BumpMapShader;
	Light* m_Light;

public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	
	bool Frame();
	bool Render();
};
#endif