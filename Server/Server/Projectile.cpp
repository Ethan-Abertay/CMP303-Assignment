#include "Projectile.h"

Projectile::Projectile(unordered_map<unsigned int, Client*>* clients_ptr, ProjectileShotPacket infoPacket, unsigned int ID)
{
	clients = clients_ptr;

	newData.position = infoPacket.position;
	newData.time = infoPacket.time;
	velocity = infoPacket.velocity;
	ownerID = ID;
}

bool Projectile::frame(float dt)
{
	// Handle timeout
	timeout -= dt;
	if (timeout <= 0)
		return false;

	// Save old packet
	oldData = newData;

	// Calculate new packet
	newData.position += velocity * dt;
	newData.time += dt;

	// Detect collisions
	const Vector3<float> projectilePos = oldData.position;	// A position on dispalcement vector's line
	Vector3<float> clientPos;			// A position on displacement vector's line
	const Vector3<float> projectileDisplacement = newData.position - oldData.position;;	// Projectile's displacement vector
	Vector3<float> clientDisplacement;	// Client's displacement vector
	Vector3<float> unitNormal;			// Unit normal between displacement vectors
	Vector3<float> unitDirection;		// The unit vector along the projectile displacement vector
	Vector3<float> knownVector;			// The vector between known points on either displacement vectors
	float shortestDistance;				// The shortest distance between displacement vecotrs
	float angle;						// The angle used for line and point
	bool withinBounds;					// If the shortest distance intersection is within line segment boundaries
	//Vector3<float> intersection;		// The point at which the shortest point intersects the projectile displacement vector
	float t;							// The t in the line equation r = a + tb for intersection 

	// Lambda for when client is stationary
	auto lineAndPoint = [&]()
	{
		// Get known vector
		knownVector = projectilePos - clientPos;

		// Get angle 
		angle = knownVector.angle(projectileDisplacement);	// This function could be made more efficient

		// Use simple trig. to get shortest distance
		shortestDistance = knownVector.length() * sinf(angle);

		// Get shortest distance intersection point
		// Distance along / total distance of line segment
		t = knownVector.length() * cosf(angle) / projectileDisplacement.length();

		// Determine if within bounds
		if (t >= 0 && t <= 1)
			withinBounds = true;
		else
			withinBounds = false;
	};
	// Lambda for when client is not stationary
	auto lineAndLine = [&]()
	{
		// Get unit normal between displacement vectors
		unitNormal = projectileDisplacement.cross(clientDisplacement);
		unitNormal.normalized();

		// Get vector between known points on displacement lines
		knownVector = projectilePos - clientPos;

		// Get shortest distance
		shortestDistance = knownVector.dot(unitNormal);

		// Get unit vector along projectile displacement
		unitDirection = projectileDisplacement;
		unitDirection.normalized();

		// Get shortest distance intersection point
		// Distance along / total distance of line segment
		t = knownVector.dot(unitDirection) / projectileDisplacement.length();

		// Determine if within bounds
		if (t >= 0 && t <= 1)
			withinBounds = true;
		else
			withinBounds = false;
	};
	// For loop
	for (auto it = clients->begin(); it != clients->end(); it++)
	{
		if (it->first != ownerID)	// Don't detect collision on projetile's owner
		{
			// Get client variables
			auto* secondLastPacket = it->second->getSecondLastInfoPacket();
			if (!secondLastPacket)
				continue;
			clientPos = secondLastPacket->position;
			clientDisplacement = it->second->getLatestInfoPacket()->position - clientPos;	// If there is a second last packet then there is a last, no need to check if null

			// Determine if client is stationary or not
			if (clientDisplacement.sum() == 0)
				lineAndPoint();
			else
				lineAndLine();

			// If shortest distance is within both radii
			if (shortestDistance <= playerRadius + projectileRadius)
			{
				// Determine if shortest distance intersection is within the line segment boundaries
				if (withinBounds)
				{
					// If using a high tick rate server and connection quality is good than this will be sufficient hit detection
					// However, if the gap between the two client packets is large than the shortest distance may be 0 but it should still miss
					printf("Hit!\n");
				}
			}
		}
	}

	return true;
}
