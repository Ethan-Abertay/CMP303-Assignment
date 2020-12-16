#pragma once
#include "PlayerMesh.h"
#include "HighLevelMesh.h"


// Forward declerations
template<class T> class Vector3;

class Enemy
{
public:
	Enemy(ID3D11Device* device);
	~Enemy();

	void updatePosition(Vector3<float> newPos);

	HighLevelMesh* mesh;

	const XMMATRIX meshTranslation = XMMatrixTranslation(0.f, -3.f, 0.f);		// Translates the mesh to the correct position
};

