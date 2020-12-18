#include "Enemy.h"
#include "../../ServerClientHeader.h"

Enemy::Enemy(ID3D11Device* device)
{
	// Initialise meshes
	mesh = new HighLevelMesh(new PlayerMesh(device, "res/playerModel.obj", XMFLOAT3(1.f, 0.25f, 0.25f)));
	mesh->addWorldMatrix(XMMatrixIdentity());

	// Initialise deque
	infoPackets = new deque<InfoPacket>;
}

Enemy::~Enemy()
{
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (infoPackets)
	{
		delete infoPackets;
		infoPackets = 0;
	}
}

void Enemy::frame(float dt, float serverTime)
{
	// Increment timer
	timeSinceLastMessage += dt;

	// Lambda for interpolating
	auto interpolate = [&]()
	{
		// Ensure there's enough data to interpolate with
		if (infoPackets->size() > 1)
		{
			// Get adjusted time
			const float adjustedTime = serverTime - INTERP_BUFFER_TIME;

			// Determine which packets to use for interpolation
			InfoPacket* oldPacket = 0, * newPacket = 0;
			for (int i = infoPackets->size() - 1; i > 0; i--)
			{
				if ((*infoPackets)[i].time < adjustedTime || i == 0)
				{
					if (i + 1 < infoPackets->size())
					{
						oldPacket = &(*infoPackets)[i];
						newPacket = &(*infoPackets)[i + 1];
					}
					break;
				}
			}

			// If still nullptrs than it's time to extrapolate
			if (oldPacket == 0 || newPacket == 0)
			{
				oldPacket = &(*infoPackets)[infoPackets->size() - 2];
				newPacket = &infoPackets->back();
			}

			// Interpolate between packets
			InfoPacket interpPacket = Interp::interpolate(*oldPacket, *newPacket, adjustedTime);

			// Set new position
			const XMFLOAT3 newPos = XMFLOAT3(interpPacket.position.x, interpPacket.position.y, interpPacket.position.z);
			setPosition(newPos);
		}
	};

	// Figure out if interpolating or extrapolating (predicting)
	if (infoPackets->back().time > serverTime)
	{
		// We are interpolating
		interpolate();
	}
	else
	{
		// We are extrapolating
		// Determine if we are within extrapolation range
		if (serverTime <= infoPackets->back().time + EXTRAP_TIME_MAX)
		{
			// We are extrapolating
			// Just use same linear interpolation function
			interpolate();
		}
		// Else do not update the position anymore until packets are received again
	}
}

void Enemy::newInfoUpdate(InfoPacket& packet)
{
	// Reset timer
	timeSinceLastMessage = 0.f;

	// Detect if this is a duplicate package
	if (infoPackets->size() > 1)
		if (packet.time == infoPackets->back().time)
			return;

	// Push new update onto end of deque
	infoPackets->push_back(packet);

	// Get rid of old updates
	while (true)
	{
		// Detect if oldest update is old enough to expire
		if (infoPackets->back().time - infoPackets->front().time >= expireTime)
		{
			infoPackets->pop_front();
		}
		else
		{
			break;
		}
	}
}

void Enemy::setPosition(XMFLOAT3 newPos)
{
	XMMATRIX worldMatrix = XMMatrixIdentity();
	worldMatrix *= XMMatrixTranslation(newPos.x, newPos.y, newPos.z);
	worldMatrix *= meshTranslation;

	mesh->setWorldMatrix(worldMatrix);
}
