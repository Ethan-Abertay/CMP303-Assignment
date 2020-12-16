#include "Enemy.h"
#include "NetworkManager.h"

Enemy::Enemy(ID3D11Device* device)
{
	// Initialise meshes
	mesh = new HighLevelMesh(new PlayerMesh(device, "res/playerModel.obj", XMFLOAT3(1.f, 0.25f, 0.25f)));
	mesh->addWorldMatrix(XMMatrixIdentity());
}

Enemy::~Enemy()
{
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}
}

void Enemy::updatePosition(Vector3<float> newPos)
{
	XMMATRIX worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixTranslation(newPos.x, newPos.y, newPos.z);
	worldMatrix *= meshTranslation;

	mesh->setWorldMatrix(worldMatrix);
}
