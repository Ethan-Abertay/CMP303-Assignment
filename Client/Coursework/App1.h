// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "DepthShader.h"
#include "TextureShader.h"
#include <vector>

// My classes
#include "XM_Math.h"
#include "HighLevelMesh.h"
#include "TessellatablePlane.h"
#include "NetworkManager.h"

// Using declerations
using std::vector;

// Defines
#define SMAP_SIZE 2048.f    /* The dimensions of the shadow maps */
#define LIGHT_NEAR 2.f
#define LIGHT_FAR 400.f

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	// Functions
	void renderMeshes();
	void updateDirectionalLightPositions();
	void updatePlayerSpotlight();

	// Meshes
	vector<HighLevelMesh*> normalMeshes;	// All generic meshes
	vector<HighLevelMesh*> tessMeshes;		// All meshes that use the tessellation shaders
	vector<Enemy*> enemies;					// All enemies to render

	// Player
	Player* player;

	// Networking
	NetworkManager* networkManager;

	// Shaders
	SceneShader* sceneShader = 0;
	DepthShader* depthShader = 0;
	TessellatedHeightMapShader* tessHeightMapShader = 0;
	TessHeightMapDepthShader* tessHeightMapDepthShader = 0;
	TextureShader* textureShader = 0;
	SimpleColourShader* colourShader = 0;

	// Lights
	vector<Light*> lights;
	float spotlightExponent = 50.f;
	float specularPower = 50.f;
	XMFLOAT3 diffuseColour = XMFLOAT3(1.f, 1.f, 1.f);
	XMFLOAT4 specularColour = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	float attenConst = 1.f;
	float attenLinear = 0.f;
	float attenQuadratic = 0.f;

	// Misc.
	vector<ShadowMap*> shadowMaps;
	vector<ShadowMap*> pointShadowMaps;
	int lightOrthoWidth = 0, lightOrthoHeight = 0;
	bool bRenderShadowMapOrtho = true;
	bool bRenderRayPassOrtho = true;
	int shadowMapToRender = 0;
	float m_screenHeight = 0, m_screenWidth = 0;	// Fullscreen resolution
	float m_postProcessHeight = 0, m_postProcessWidth = 0;	// The resolution of the post process

	// Trig shader variables
	bool bPauseTime = false;
	float time = 10;
	float timeRate = 0.4f;
	float frequency = 0.6f;
	float amplitude = 5.f;

	// Ground variables that GUI can change
	int *maxTessellationFactor = 0;
	int *maxTessDistance = 0;
	float *maxHeight = 0;
};

#endif