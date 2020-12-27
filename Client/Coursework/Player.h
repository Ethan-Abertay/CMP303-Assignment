#pragma once
#include "PlayerMesh.h"
#include "DXF.h"
#include "XM_Math.h"

// Camera accesses this class so Network Manager must be forward declared
class NetworkManager;

class Player
{
public:
	Player(FPCamera* cam_ptr);
	~Player();

	void init(NetworkManager* network_ptr);
	void frame(float dt);

	void shoot(XMFLOAT3 position, XMFLOAT3 forward);
	bool canFire();
	XMFLOAT3 getFireVelocity(XMFLOAT3 input);

	XMFLOAT3 getPosition() { return camera->getPosition(); };

	void setPosition(float x, float y, float z) { camera->setPosition(x, y, z); };

private:
	FPCamera* camera;
	NetworkManager* networkManager;

	// Firing variables
	float fireDelay = 0.25f;
	float fireDelayTimer = 0.f;
	float fireVelocity = 75.f;
};

