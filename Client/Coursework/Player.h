#pragma once
#include "PlayerMesh.h"
#include "DXF.h"
#include "XM_Math.h"

class Player
{
public:
	Player(FPCamera* cam_ptr);
	~Player();

	void frame(float dt);

	bool canFire();
	XMFLOAT3 getFireVelocity(XMFLOAT3 input);

	XMFLOAT3 getPosition() { return camera->getPosition(); };

private:
	FPCamera* camera;

	// Firing variables
	float fireDelay = 0.1f;
	float fireDelayTimer = 0.f;
	float fireVelocity = 5.f;
};

