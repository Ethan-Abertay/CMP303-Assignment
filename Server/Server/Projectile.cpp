#include "Projectile.h"

Projectile::Projectile(unordered_map<unsigned int, Client*>* clients_ptr, ProjectileShotPacket infoPacket, unsigned int ID)
{
	clients = clients_ptr;

	newData.position = infoPacket.position;
	newData.time = infoPacket.time;
	velocity = infoPacket.velocity;
	ownerID = ID;
	startTime = infoPacket.time;
}

bool Projectile::frame(float dt, float time)
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

	// Variables for detection 
	Client::RelevantPackets packets;
	const Vector3<float> projectilePos = oldData.position;	// A position on dispalcement vector's line
	Vector3<float> clientPos;			// A position on displacement vector's line
	const Vector3<float> projectileDisplacement = newData.position - oldData.position;;	// Projectile's displacement vector
	Vector3<float> clientDisplacement;	// Client's displacement vector
	Vector3<float> knownVector;			// The vector between known points on either displacement vectors
	float shortestDistance;				// The shortest distance between displacement vecotrs
	float angle;						// The angle used for line and point
	bool withinBounds;					// If the shortest distance intersection is within line segment boundaries
	//Vector3<float> intersection;		// The point at which the shortest point intersects the projectile displacement vector

	// Lambda for when client is stationary
	float t;
	auto lineAndPoint = [&]()
	{
		// Get known vector
		knownVector = clientPos - projectilePos;

		// Get angle 
		angle = knownVector.angle(projectileDisplacement);	// This function could be made more efficient

		// Use simple trig. to get shortest distance
		shortestDistance = fabsf(knownVector.length() * sinf(angle));

		// Get shortest distance intersection point
		// Distance along / total distance of line segment
		t = knownVector.length() * cosf(angle) / projectileDisplacement.length();

		// Determine if within bounds
		if (t >= 0.f && t <= 1.f)
			withinBounds = true;
		else
			withinBounds = false;
	};
	// Lambda for when client is not stationary
	float a, b, c, d, e, sc, tc;
	Vector3<float> u, v, w0, p0, q0, W;
	auto lineAndLine = [&]()
	{
		// A lot of this code was adapted from the following
		// http://geomalgorithms.com/a07-_distance.html 
		// This method allows the line between two lines' (which is the shortest) intersection of the 2 original lines to be easily found
		// My previous method did not do this which had serious limitations 

		// Set line direction vectors
		u = projectileDisplacement;
		v = clientDisplacement;

		// Get line start positions
		p0 = projectilePos;
		q0 = clientPos;
		w0 = p0 - q0;

		// Get simutaneous equations params
		// See reference above to understand how to get equation
		// Fundamentally works because the shortest line is perpendicular to both and dot product of both is hence 0, allows for sim. equations
		a = u.dot(u);
		b = u.dot(v);
		c = v.dot(v);
		d = u.dot(w0);
		e = v.dot(w0);

		// Get sc and tc for W(sc, tc) line (shortest line) from sim. equations
		sc = ((b*e) - (c*d)) / ((a*c) - (b*b));
		tc = ((a*e) - (b*d)) / ((a*c) - (b*b));

		// Determine if within bounds of projectile line
		if (sc >= 0.f && sc <= 1.f)
		{
			withinBounds = true;

			// Get shortest distance to line segment i.e. 0 <= tc <= 1
			if (tc < 0.f)
				tc = 0.f;
			if (tc > 1.f)
				tc = 1.f;

			// Get shortest distance (magnitude of W(sc, tc))
			W = w0 + u * sc - v * tc;
			shortestDistance = W.length();
		}
		else
			withinBounds = false;
	};
	// For loop
	for (auto it = clients->begin(); it != clients->end(); it++)
	{
		if (it->first != ownerID)	// Don't detect collision on projetile's owner
		{
			// Get client variables
			float adjustedTime = time - INTERP_BUFFER_TIME;
			packets = it->second->getRelaventPackets(adjustedTime);
			if (!packets.oldPacket || !packets.newPacket)
				continue;
			clientPos = packets.oldPacket->position;
			clientDisplacement = packets.newPacket->position - clientPos;	// If there is a second last packet then there is a last, no need to check if null

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
					printf("Client %i was hit by a projectile from client %i\n", it->first, ownerID);	// Output message to cmd

					// Flag client as hit
					it->second->bHit = true;
				}
			}
		}
	}

	return true;
}
