#pragma once
#include "PlayerMesh.h"
#include "HighLevelMesh.h"

#include <deque>

// Using declerations
using std::deque;

// Forward declerations
class InfoPacket;

class Enemy
{
public:
	Enemy(ID3D11Device* device);
	~Enemy();

	void frame(float dt, float serverTime);

	void newInfoUpdate(InfoPacket& packet);

	HighLevelMesh* mesh;

	float timeSinceLastMessage = 0.f;

private:
	void setPosition(XMFLOAT3 newPos);

	deque<InfoPacket> *infoPackets;	// Must be pointer because InfoPacket is forward decelared in header
	const float expireTime = 1.f;

	const XMMATRIX meshTranslation = XMMatrixTranslation(0.f, -1.f, 0.f);		// Translates the mesh to the correct position
};

