#pragma once
#include "PlayerMesh.h"
#include "HighLevelMesh.h"

class Enemy
{
public:
	Enemy(ID3D11Device* device);
	~Enemy();

	HighLevelMesh* mesh;

private:

};

