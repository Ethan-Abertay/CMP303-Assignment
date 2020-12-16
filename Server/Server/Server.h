#pragma once

#include <cassert>
#include <thread>
#include <Windows.h>	// Contains Sleep()
#include <vector>
#include <unordered_map>

#include "Client.h"

using std::thread;
using std::vector;
using std::unordered_map;

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

	// Auxiliary functions
	void resetMsgBuffer(int size = maxMessageSize) { memset(&msg_buffer, 0, size); };
	void sendJoinAcceptMessage(int ID);
	template<class A, class B> bool keyExists(unordered_map<A, B> &map_, A key);

	// Other functions
	void ping(string serverIP);
	void killThreads();

	// Network variables
	sf::UdpSocket socket;
	unordered_map<unsigned int, Client*> clients;
	char msg_buffer[maxMessageSize];

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
