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
#include "RenderTexture.h"
#include "Bitmap.h"
#include "FadeShader.h"


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
	TextureShader* m_TextureShader;
	RenderTexture* m_RenderTexture;
	Bitmap* m_Bitmap;
	FadeShader* m_FadeShader;
	float m_fadeInTime;
	float m_accumulatedTime;
	float m_fadePercentage;
	bool m_fadeDone;

public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame(float frameTime);
	bool Render();

private:
	bool RenderToTexture(float rotation);
	bool RenderFadingScreen();
	bool RenderNormalScene(float rotation);
};
#endif