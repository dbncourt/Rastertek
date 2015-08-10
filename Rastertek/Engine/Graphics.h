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
#include "DepthShader.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;


////////////////////////////////////////////////////////////////////////////////
// Class name: Graphics
////////////////////////////////////////////////////////////////////////////////
class Graphics
{

private:
	Direct3D* m_Direct3D;
	Camera* m_Camera;
	Model* m_Model;
	DepthShader* m_DepthShader;

public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();
};
#endif