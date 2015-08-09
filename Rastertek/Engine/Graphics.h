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
#include "TextureShader.h"


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
	Model* m_FloorModel;
	Model* m_BillboardModel;
	TextureShader* m_TextureShader;

public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame(D3DXVECTOR3 position);

private:
	bool Render();
};
#endif