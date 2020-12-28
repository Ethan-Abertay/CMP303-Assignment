#pragma once
#include <SFML/Network.hpp>

#include "../../ServerClientHeader.h"
#include "Enemy.h"
#include "Player.h"

#include <unordered_map>
#include <thread>
#include <chrono>		// Used for ping timing

// Using declerations
using std::unordered_map;
using std::thread;
using std::chrono::time_point;

typedef std::chrono::high_resolution_clock chronoClock;

// Forward declerations
class App1;

class NetworkManager
{
public:
	NetworkManager(App1* app1_ptr, Player* player_ptr);
	~NetworkManager();

	void frame(float dt);

	void newServer(string newIP, uint32_t newPort);
	void disconnect();
	void shoot(XMFLOAT3 position, XMFLOAT3 forward);

	// Getters
	float getServerTime() { return currentTime; };

private:
	// Network functions
	void receiveMessages();
	void sendJoinRequestMessage();
	void receiveJoinAcceptMessage();
	void sendPing();
	bool receivePing();		// Returns true if the ping was received
	void sendUpdateInfo();

	// Auxiliary Functions
	void resetMsgBuffer(int size = PACKET_LIMIT) { memset(&msg_buffer, 0, size); };
	Vector3<float> XMToVec3(XMFLOAT3 f3);
	XMFLOAT3 Vec3ToXM(Vector3<float> v3);
	template<class A, class B> bool keyExists(unordered_map<A, B>& map_, A key);

	// Other functions
	void ping();
	void manageEnemies();
	void spawnProjectile(ProjectileShotPacket &info);

	// Class pointers
	App1* app1;
	Player* player;
	unordered_map<unsigned int, Enemy*> enemies;		// Not to be deleted in this class

	// Server connection variables
	sf::UdpSocket socket;
	string serverIP = "";
	uint32_t port = 0;
	unsigned int ID;
	unsigned short pingPort;	// The port of the server to ping 
	char msg_buffer[PACKET_LIMIT];
	bool bConnected = false;	// True is successfully connected to server
	float currentTime = 0;		// The time that is synchronised with the server

	// Establishing connection variables
	bool bWaitingToEstablishConnection = false;	// Whether or not the client is expecting a message confirming a request join message
	float establishConnectionTimeout = 5.0f;	// The time taken to send another request join message (in seconds)
	float establishConnectionTimer = 0;

	// Ping related variables
	thread* pingThread = 0;			// Thread used to find the ping
	bool bSentPing = false;			// Whether not the client has sent a ping request or not
	const float pingTimeout = 5.f;		// The time taken to send another ping (in seconds)
	float pingTimer = 0;				// The current timer for ping timeout
	const int noOfTimesToPing = 10;		// The number of times to ping the server to adjust the current time
	int currentPingNo = 0;				// The current ping number
	float pingSum = 0;
	float pingAdjustment = 0;

	// Network variables
	float tickDelayMax = 1.f / (float)CLIENT_TICK_RATE;	// The delay between ticks in seconds
	float tickDelayTimer = 0;
};

template<class A, class B>
bool NetworkManager::keyExists(unordered_map<A, B>& map_, A key)
{
	if (map_.find(key) == map_.end())
		return false;
	return true;
}
