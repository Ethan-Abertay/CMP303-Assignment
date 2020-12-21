#pragma once

#include <cassert>
#include <thread>
#include <Windows.h>	// Contains Sleep()
#include <vector>
#include <unordered_map>
#include <list>

#include "Client.h"
#include "Projectile.h"

using std::thread;
using std::vector;
using std::unordered_map;
using std::list;

class Server
{
public:
	Server();
	~Server();

	void init(string serverIP, int port);

private:
	// Main execution functions
	void run();
	void receiveMessages();
	void sendMessages();
	void manageClients(float dt);
	void manageProjectiles(float dt);

	// Send functions
	void sendJoinAcceptMessage(int ID);
	void sendUpdateInfo(Client* enemy, Client* player);		// Data about enemy is sent to player

	// Auxiliary functions
	void resetMsgBuffer(int size = PACKET_LIMIT) { memset(&msg_buffer, 0, size); };
	template<class A, class B> bool keyExists(unordered_map<A, B> &map_, A key);

	// Other functions
	void ping(string serverIP);
	void killThreads();

	// Network variables
	sf::UdpSocket socket;
	unordered_map<unsigned int, Client*> clients;	// Map used for constant time access by key
	list<Projectile*> projectiles;				// Forward list used for fast random deletion 
	char msg_buffer[PACKET_LIMIT];

	// Timing variables
	sf::Clock clock;
	float currentTime;	// SFML uses float for time in seconds (total of 2^32 = 4294967296 seconds enough seconds for ~1.35 centuries of seconds)
	float dt;			// Delta time

	// Server operating variables
	const float tickDelay = 1.f / (float)SERVER_TICK_RATE * 1000.f;	// The delay between ticks in milliseconds
	bool bExecuting = true;		// Once set false all threads should stop executing

	// Threads
	thread *pingThread;	// The thread that monitors the ping port for ping requests
};
