#pragma once
#include "PlayerMesh.h"
#include "DXF.h"

class Player
{
public:
	Player(FPCamera* cam_ptr);
	~Player();

	XMFLOAT3 getPosition() { return camera->getPosition(); };

private:
	FPCamera* camera;

};

