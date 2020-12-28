// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Set variables
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	// Set post process to quater the resolution total
	m_postProcessWidth = screenWidth / 2;
	m_postProcessHeight = screenHeight / 2;

	// Handle camera
	camera->bWalkMode = true;	// Set camera to walk mode


	// Initialise player
	player = new Player(camera);

	// Initialise network manager
	networkManager = new NetworkManager(this, player);
	player->init(networkManager);

	// Initialise shaders
	sceneShader = new SceneShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	tessHeightMapShader = new TessellatedHeightMapShader(renderer->getDevice(), hwnd);
	tessHeightMapDepthShader = new TessHeightMapDepthShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	colourShader = new SimpleColourShader(renderer->getDevice(), hwnd);

	// Get textures
	textureMgr->loadTexture(L"wall", L"res/WallAlbedo.tif");
	textureMgr->loadTexture(L"wallDisplacement", L"res/WallDisplacement.tif");
	textureMgr->loadTexture(L"wallNormal", L"res/WallNormal.tif");
	textureMgr->loadTexture(L"rockSoil", L"res/RockSoil_Albedo.tif");
	textureMgr->loadTexture(L"rockSoilDisplacement", L"res/RockSoil_Displacement.tif");
	textureMgr->loadTexture(L"rockSoilNormal", L"res/RockSoil_Normal.tif");

	// Initalise meshes
	// Tessellated walls
	HighLevelMesh* wall = new HighLevelMesh(new TessellatablePlane(renderer->getDevice(), renderer->getDeviceContext(), XMFLOAT2(100, 6), XMINT2(35, 4), XMFLOAT2(10.f, 1.f)));	// The mesh
	// Get a bunch of transformation matrices
	XMMATRIX x270 = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 1.f), XMConvertToRadians(270.f));
	XMMATRIX x90 = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 1.f), XMConvertToRadians(90.f));
	XMMATRIX y180 = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(180.f));
	XMMATRIX y90 = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
	XMMATRIX y270 = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(270.f));
	// Create different world matrices (spawns a new instance of this model modified a world matrix)
	wall->addWorldMatrix(x270 * XMMatrixTranslation(-50.f, -1.f, 50.f));			// Front
	//wall->addRotationMatrix(x270);
	wall->addWorldMatrix(y180 * x90 * XMMatrixTranslation(50.f, -1.f, -50.f));		// Back
	//wall->addRotationMatrix(y180 * x90);
	wall->addWorldMatrix(x270 * y90 * XMMatrixTranslation(50.f, -1.f, 50.f));		// Right
	//wall->addRotationMatrix(x270 * y90);
	wall->addWorldMatrix(x270 * y270 * XMMatrixTranslation(-50.f, -1.f, -50.f));	// Left
	//wall->addRotationMatrix(x270 * y270);
	// Apply textures
	wall->setTexture(textureMgr->getTexture(L"wall"));
	wall->setHeightmap(textureMgr->getTexture(L"wallDisplacement"));
	wall->setNormalMap(textureMgr->getTexture(L"wallNormal"));
	// Set tessellation variables to desired values
	wall->setHeightMapDisplacementMax(1.5f);
	wall->setMaxTessDistance(20);
	wall->setMaxTessFactor(64);
	tessMeshes.push_back(wall);
	// Tessellated ground
	HighLevelMesh* ground = new HighLevelMesh(new TessellatablePlane(renderer->getDevice(), renderer->getDeviceContext(), XMFLOAT2(100, 100), XMINT2(100, 100), XMFLOAT2(10.f, 10.f)));
	ground->addWorldMatrix(XMMatrixTranslation(-50.f, -1.f, -50.f));
	//ground->addRotationMatrix(XMMatrixIdentity());
	ground->setTexture(textureMgr->getTexture(L"rockSoil"));
	ground->setHeightmap(textureMgr->getTexture(L"rockSoilDisplacement"));
	ground->setNormalMap(textureMgr->getTexture(L"rockSoilNormal"));
	ground->setHeightMapDisplacementMax(0.75f);
	ground->setMaxTessDistance(15);
	ground->setMaxTessFactor(50);
	// Get pointers to variables to ImGui can modify them
	maxTessellationFactor = ground->getMaxTessFactor_ptr();
	maxTessDistance = ground->getMaxTessDistance_ptr();
	maxHeight = ground->getHeightMapDisplacement_ptr();
	tessMeshes.push_back(ground);

	// Initialise shadow maps (vector allows for easier passing as parameter)
	int shadowmapWidth = SMAP_SIZE;
	int shadowmapHeight = SMAP_SIZE;
	for (int i = 0; i < MAX_LIGHTS; i++)
		shadowMaps.push_back(new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight));
	for (int i = 0; i < MAX_POINTLIGHTS * 6; i++)	// Each pointlight has 6 shadow maps
		pointShadowMaps.push_back(new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight));

	// Initialise lights
	lightOrthoWidth = 200;
	lightOrthoHeight = 200;
	// Only the first light's ambient is used to prevent issues that can arise from multiple ambient light values
	// Spotlight
	lights.push_back(new Light());
	lights.back()->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	lights.back()->setLightType(Light::LightType::SPOTLIGHT);
	lights.back()->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	lights.back()->setDiffuseColour(diffuseColour.x, diffuseColour.y, diffuseColour.z, 1.0f);
	lights.back()->setSpotlightExponent(spotlightExponent);
	lights.back()->setSpecularPower(specularPower);
	lights.back()->setSpecularColour(specularColour.x, specularColour.y, specularColour.z, specularColour.w);
	lights.back()->setAttenConst(attenConst);
	lights.back()->setAttenLinear(attenLinear);
	lights.back()->setAttenQuadratic(attenQuadratic);
	lights.back()->generateProjectionMatrix(LIGHT_NEAR, LIGHT_FAR);

	// Directional
	lights.push_back(new Light());
	lights.back()->setLightType(Light::LightType::DIRECTIONAL);
	lights.back()->setDiffuseColour(0.8f, 0.8f, 0.8f, 1.0f);
	lights.back()->setDirection(-0.5f, -0.1f, 0.5f);
	lights.back()->setSpecularPower(specularPower);
	lights.back()->setSpecularColour(specularColour.x, specularColour.y, specularColour.z, specularColour.w);
	lights.back()->setAttenConst(0);
	lights.back()->setAttenLinear(0);
	lights.back()->setAttenQuadratic(0);
	lights.back()->generateOrthoMatrix((float)lightOrthoWidth, (float)lightOrthoHeight, LIGHT_NEAR, LIGHT_FAR);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

	// Delete the rest
	if (sceneShader)
	{
		delete sceneShader;
		sceneShader = 0;
	}
	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}
	if (tessHeightMapShader)
	{
		delete tessHeightMapShader;
		tessHeightMapShader = 0;
	}
	if (tessHeightMapDepthShader)
	{
		delete tessHeightMapDepthShader;
		tessHeightMapDepthShader = 0;
	}
	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}
	if (colourShader)
	{
		delete colourShader;
		colourShader = 0;
	}
	if (networkManager)
	{
		delete networkManager;
		networkManager = 0;
	}

	// Delete lights
	for (int i = 0; i < lights.size(); i++)
	{
		delete lights[i];
		lights[i] = 0;
	}
	lights.clear();

	// Delete shadow maps
	for (int i = 0; i < shadowMaps.size(); i++)
	{
		delete shadowMaps[i];
		shadowMaps[i] = 0;
	}
	shadowMaps.clear();

	// Delete meshes
	for (int i = 0; i < normalMeshes.size(); i++)
	{
		delete normalMeshes[i];
		normalMeshes[i] = 0;
	}
	normalMeshes.clear();
	for (int i = 0; i < tessMeshes.size(); i++)
	{
		delete tessMeshes[i];
		tessMeshes[i] = 0;
	}
	tessMeshes.clear();

	// Destroy enemies
	for (int i = 0; i < enemies.size(); i++)
	{
		delete enemies[i];
		enemies[i] = 0;
	}
	enemies.clear();

	// Delete player
	if (player)
	{
		delete player;
		player = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Get dt
	const float dt = timer->getTime();

	// Update player (before or after doesn't currently matter)
	player->frame(dt);

	// Update projectiles (they are just visuals on client, hit reg is server side)
	for (auto it = projectiles.begin(); it != projectiles.end();)
	{
		if (!(*it)->frame(dt))
		{
			it = projectiles.erase(it);
		}
		else
			it++;
	}

	// Update networking before rendering
	networkManager->frame(dt);

	// Update enemies with latest network info
	const float serverTime = networkManager->getServerTime();	// Get the latest server time
	for (int i = 0; i < enemies.size(); i++)
		enemies[i]->frame(dt, serverTime);

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

Enemy* App1::createEnemy(float pingAdjust)
{
	// Intialise enemy
	Enemy* newEnemy = new Enemy(renderer->getDevice(), pingAdjust);		// This object instance (App1) will destroy the enemies
	enemies.push_back(newEnemy);							// Add this enemy to the vector
	return newEnemy;	// Return the enemy 
}

void App1::deleteEnemy(Enemy* enemy)
{
	for (int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i] == enemy)	// If this vector element points to the correct enemy to delete
		{
			enemies.erase(enemies.begin() + i);
			break;
		}
	}
}

void App1::deleteAllEnemies()
{
	for (int i = 0; i < enemies.size(); i++)
	{
		delete enemies[i];
		enemies[i] = 0;
	}
	enemies.clear();
}

void App1::newProjectile(XMFLOAT3 pos, XMFLOAT3 vel, float timeout)
{
	Projectile *projectile = new Projectile(new SphereMesh(renderer->getDevice(), renderer->getDeviceContext()), pos, vel, timeout);
	projectile->addWorldMatrix(XMMatrixIdentity());
	projectiles.push_front(projectile);
}

bool App1::render()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// updates the position of directional lights to get shadow information
	updateDirectionalLightPositions();

	// Update other misc. variables
	if (!bPauseTime)
		time += timer->getTime() * timeRate;

	// Update player's spotlight 
	updatePlayerSpotlight();

	// Render meshes
	renderMeshes();

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Begin("Info");

	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::End();

	ImGui::Begin("Networking");
	

	char input[16];
	memset(&input[0], '\0', 16);
	memcpy(&input[0], serverIP.c_str(), serverIP.size());
	if (ImGui::InputText("Server IP", &input[0], 16))
	{
		serverIP = &input[0];
	}
	ImGui::InputInt("Server port", &serverPort);
	if (ImGui::Button("Connect to specified server"))
	{
		networkManager->newServer(serverIP, serverPort);
	}
	if (ImGui::Button("Connect to Local Host"))
	{
		networkManager->newServer(LOCAL_IP, LOCAL_PORT);
	}
	if (ImGui::Button("Disconnect"))
	{
		networkManager->disconnect();
	}

	ImGui::End();

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void App1::renderMeshes()
{
	XMFLOAT3 camPos = camera->getPosition();

	auto depthPass = [&]()
	{
		TessHeightMapDepthShader::TessDepthShaderParams tessParams;
		tessParams.camPos = &camPos;
		tessParams.camPos = &camPos;

		auto renderScene = [&](XMMATRIX& view, XMMATRIX& proj)
		{
			// Render meshes
			for (int i = 0; i < normalMeshes.size(); i++)
				normalMeshes[i]->depthRender(depthShader, renderer, view, proj);

			// I know that only tessMeshes have 1 sided planes that can cause issues with backface culling and shadow maps
			// Therefore I can disable back face culling for these meshes only to ensure the shadow maps are correct

			// Disable back face culling
			renderer->disableBackFaceCulling();

			for (int i = 0; i < tessMeshes.size(); i++)
				tessMeshes[i]->depthRender(tessHeightMapDepthShader, tessParams, renderer, view, proj);

			// Enable back face culling again
			renderer->enableBackFaceCulling();
		};
		auto renderLight = [&](Light* light, int index = 0)
		{
			// Get matrices
			XMMATRIX lightViewMatrix = light->getViewMatrix(index);
			XMMATRIX lightProjectionMatrix;
			
			if (light->getLightType() == Light::LightType::DIRECTIONAL)
				lightProjectionMatrix = light->getOrthoMatrix();
			else
				lightProjectionMatrix = light->getProjectionMatrix();

			renderScene(lightViewMatrix, lightProjectionMatrix);
		};

		// Get shadow map for each light
		int pointIndex = 0;
		for (int i = 0; i < lights.size(); i++)
		{
			if (lights[i]->getLightType() != Light::LightType::POINT)
			{
				shadowMaps[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
				renderLight(lights[i]);
			}
			else
			{
				// Get all 6 point light shadow maps
				for (int j = 0; j < 6; j++)
				{
					pointShadowMaps[pointIndex]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
					renderLight(lights[i], j);
					pointIndex++;
				}
			}
		}

		// Set back buffer as render target and reset view port.
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	};

	auto finalPass = [&]()
	{
		// Get matrices
		XMMATRIX viewMatrix = camera->getViewMatrix();
		XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

		// Setup shader parameters
		SceneShader::ShaderParams params;
		params.deviceContext = renderer->getDeviceContext();
		params.view = &viewMatrix;
		params.projection = &projectionMatrix;
		params.camPosition = &camPos;
		params.lights = &lights;
		for (int i = 0; i < MAX_LIGHTS; i++)
			params.shadowMaps.push_back(shadowMaps[i]->getDepthMapSRV());
		for (int i = 0; i < MAX_POINTLIGHTS * 6; i++)
			params.pointShadowMaps.push_back(pointShadowMaps[i]->getDepthMapSRV());

		TessellatedHeightMapShader::TessShaderParams tessParams;
		tessParams.shaderParams = &params;
		tessParams.camPos = &camPos;

		SimpleColourShader::Params colourParams;
		colourParams.deviceContext = renderer->getDeviceContext();
		colourParams.view = &viewMatrix;
		colourParams.projection = &projectionMatrix;

		// Render meshes
		for (int i = 0; i < normalMeshes.size(); i++)
			normalMeshes[i]->finalRender(sceneShader, renderer, params);
		for (int i = 0; i < tessMeshes.size(); i++)
			tessMeshes[i]->finalRender(tessHeightMapShader, renderer, tessParams);

		// Colour of enemies
		colourParams.colour = XMFLOAT4(0.5f, 0.f, 0.f, 1.f);
		for (int i = 0; i < enemies.size(); i++)
			enemies[i]->mesh->colourRender(colourShader, renderer, colourParams);

		// Colour of projectiles
		colourParams.colour = XMFLOAT4(0.0f, 0.5f, 0.f, 1.f);
		for (auto it = projectiles.begin(); it != projectiles.end(); it++)
			(*it)->colourRender(colourShader, renderer, colourParams);
	};

	auto renderToBackBuffer = [&]()
	{
		// Reset to render to back buffer
		renderer->setBackBufferRenderTarget();

		finalPass();
	};

	// Generate view matrices
	for (int i = 0; i < lights.size(); i++)
		lights[i]->generateViewMatrix();

	depthPass();
	renderToBackBuffer();
}

void App1::updateDirectionalLightPositions()
{
	// An abstract distance along the inverted direction vector starting from the player's position
	float distanceFromPlayer = 100.f;

	for (int i = 0; i < lights.size(); i++)
	{
		if (lights[i]->getLightType() == Light::LightType::DIRECTIONAL)
		{
			XMFLOAT3 newPosition = XM_Math::add(XM_Math::multiply(XM_Math::inverse(XM_Math::normalise(lights[i]->getDirection())), distanceFromPlayer), camera->getPosition());	// Light's direction normalised, inversed and multiplied by the desired distance added onto player position
			lights[i]->setPosition(newPosition.x, newPosition.y, newPosition.z);	// Set position
		}
	}
}

void App1::updatePlayerSpotlight()
{
	// Player spotlight
	lights[0]->setPosition(camera->getPosition());
	lights[0]->setDirection(camera->getForwardVector());
	lights[0]->setDiffuseColour(diffuseColour.x, diffuseColour.y, diffuseColour.z, 1.0f);
	lights[0]->setSpotlightExponent(spotlightExponent);
	lights[0]->setSpecularPower(specularPower);
	lights[0]->setAttenConst(attenConst);
	lights[0]->setAttenLinear(attenLinear);
	lights[0]->setAttenQuadratic(attenQuadratic);
}

