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
#include "RenderTexture.h"
#include "LightShader.h"
#include "RefractionShader.h"
#include "WaterShader.h"


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
	Model* m_GroundModel;
	Model* m_WallModel;
	Model* m_BathModel;
	Model* m_WaterModel;
	Light* m_Light;
	RenderTexture* m_RefractionTexture;
	RenderTexture* m_ReflectionTexture;
	LightShader* m_LightShader;
	RefractionShader* m_RefractionShader;
	WaterShader* m_WaterShader;
	float m_waterHeight;
	float m_waterTranslation;

public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();
	bool Render();

private:
	bool RenderRefractionToTexture();
	bool RenderReflectionToTexture();
	bool RenderScene();
};
#endif