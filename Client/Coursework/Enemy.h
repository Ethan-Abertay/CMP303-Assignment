#pragma once
#include "PlayerMesh.h"
#include "HighLevelMesh.h"

#include <deque>

// Using declerations
using std::deque;

// Forward declerations
class ClientInfoPacket;

class Enemy
{
public:
	Enemy(ID3D11Device* device, float pingAdjust_);
	~Enemy();

	void frame(float dt, float serverTime);

	void newInfoUpdate(ClientInfoPacket& packet);

	HighLevelMesh* mesh;

	float timeSinceLastMessage = 0.f;

private:
	void setPosition(XMFLOAT3 newPos);

	deque<ClientInfoPacket> *infoPackets;	// Must be pointer because InfoPacket is forward decelared in header
	const float expireTime = 1.f;
	float pingAdjustment = 0.f;

	const XMMATRIX meshTranslation = XMMatrixTranslation(0.f, -1.f, 0.f);		// Translates the mesh to the correct position
};

