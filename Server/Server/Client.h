#pragma once

#include <SFML/Network.hpp>

#include "../../ServerClientHeader.h"

#include <deque>

using std::deque;

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

	// Network info
	sf::IpAddress address;
	unsigned short port;
	unsigned int ID;

	float timeSinceLastMessage = 0;
	bool bHit = false;

private:
	// Game info
	deque<ClientInfoPacket>* infoPackets = 0;


};

