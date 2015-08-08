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
#include "Light.h"
#include "LightShader.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
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
	LightShader* m_LightShader;
	Light* m_Light1;
	Light* m_Light2;
	Light* m_Light3;
	Light* m_Light4;

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