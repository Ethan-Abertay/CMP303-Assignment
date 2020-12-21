#pragma once
#include "HighLevelMesh.h"
#include "XM_Math.h"

class Projectile : public HighLevelMesh
{
public:
	Projectile(BaseMesh* mesh, XMFLOAT3 pos_, XMFLOAT3 vel_, float timeout_);

	bool frame(float dt);	// Returns false to delete this

private:
	XMFLOAT3 pos, vel;
	float timeout = 0.f;
};

