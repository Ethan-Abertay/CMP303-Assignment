#pragma once

#include <SFML/Network.hpp>

#include "../../ServerClientHeader.h"

#include <deque>
#include <unordered_map>

using std::deque;
using std::unordered_map;

class Client
{
public:
	struct RelevantPackets
	{
		RelevantPackets() {};

		ClientInfoPacket* oldPacket = 0;
		ClientInfoPacket* newPacket = 0;
	};

public:
	Client();
	Client(sf::IpAddress addrs, unsigned short port_param, unsigned int ID_param);
	~Client();

	void newInfoPacket(ClientInfoPacket& info);

	// Getters
	ClientInfoPacket* getLatestInfoPacket();
	RelevantPackets getRelaventPackets(float time);
	float getLatestPacketTime(unsigned int ID) { return latestPacketsFromClients[ID]; };

	// Setters
	void setLatestPacketTime(unsigned int ID, float time) { latestPacketsFromClients[ID] = time; };

	// Network info
	sf::IpAddress address;
	unsigned short port;
	unsigned int ID;

	float timeSinceLastMessage = 0;
	bool bHit = false;

private:
	// Game info
	deque<ClientInfoPacket>* infoPackets = 0;
	unordered_map<unsigned int, float> latestPacketsFromClients;	// Stores the time of the latest packet from each client to this one

};

