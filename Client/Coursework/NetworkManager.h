#pragma once
#include <SFML/Network.hpp>

#include "../../ServerClientHeader.h"
#include "Enemy.h"
#include "Player.h"

class NetworkManager
{
public:
	NetworkManager(Player* player_ptr);
	~NetworkManager();

	void frame(float dt);

	// Setters
	void newServer(string newIP, uint32_t newPort);

private:
	// Netowrk functions
	void sendJoinRequestMessage();
	void receiveJoinAcceptMessage();
	void sendPing();
	float receivePing();		// Returns the ping
	void sendUpdateInfo();
	void receiveUpdateInfo();

	// Auxiliary Functions
	void resetMsgBuffer(int size = maxMessageSize) { memset(&msg_buffer, 0, size); };
	Vector3<float> XMToVec3(XMFLOAT3 f3);



	Player* player;

	// Server connection variables
	sf::UdpSocket socket;
	string serverIP = "";
	uint32_t port = 0;
	unsigned int ID;
	unsigned short pingPort;	// The port of the server to ping 
	char msg_buffer[maxMessageSize];
	bool bConnected = false;	// True is successfully connected to server
	float currentTime = 0;		// The time that is synchronised with the server

	// Establishing connection variables
	bool bWaitingToEstablishConnection = false;	// Whether or not the client is expecting a message confirming a request join message
	float establishConnectionTimeout = 5.0f;	// The time taken to send another request join message (in seconds)
	float establishConnectionTimer = 0;

	// Ping related variables
	bool bAdjustingTime = false;	// Whether or not the client is trying to use the ping to adjust the time (doesn't do normal networking during this time)
	bool bSentPing = false;			// Whether not the client has sent a ping request or not
	float pingSentTime = 0;			// The time at which a ping message was sent
	float pingTimeout = 5.f;		// The time taken to send another ping (in seconds)
	float pingTimer = 0;			// The current timer for ping timeout
	int noOfTimesToPing = 10;		// The number of times to ping the server to adjust the current time
	int currentPingNo = 0;			// The current ping number
	float pingSum = 0;				// The current sum of all pings in milliseconds

	// Network variables
	float tickDelayMax = 1.f / (float)CLIENT_TICK_RATE;	// The delay between ticks in seconds
	float tickDelayTimer = 0;
};

