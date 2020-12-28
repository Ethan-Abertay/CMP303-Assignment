#pragma once
#include "Client.h"

#include <unordered_map>

using std::unordered_map;

class Projectile
{
private:
	struct Data
	{
		Vector3<float> position;
		float time;
	};

public:
	Projectile(unordered_map<unsigned int, Client*>* clients_ptr, ProjectileShotPacket infoPacket, unsigned int ID);

	bool frame(float dt, float time);

	// Getters
	float getStartTime() { return startTime; };

private:
	unordered_map<unsigned int, Client*>* clients;	// A pointer to the clients map in the server.h

	// Projectile variables
	Data oldData, newData;
	Vector3<float> velocity;
	float startTime;
	unsigned int ownerID;
	float timeout = PROJECTILE_TIMEOUT;

	// Hitbox variables
	const float playerRadius = 1.f;
	const float projectileRadius = 1.f;
};

