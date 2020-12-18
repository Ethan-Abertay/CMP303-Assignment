#pragma once

#include <SFML/Network.hpp>

#include "../../ServerClientHeader.h"

#include <deque>

using std::deque;

class Client
{
public:
	Client();
	Client(sf::IpAddress addrs, unsigned short port_param, unsigned int ID_param);
	~Client();

	void newInfoPacket(InfoPacket& info);

	// Getters
	InfoPacket& getLatestInfoPacket() { return infoPackets->back(); };

	// Network info
	sf::IpAddress address;
	unsigned short port;
	unsigned int ID;

	float timeSinceLastMessage = 0;

private:
	// Game info
	deque<InfoPacket>* infoPackets = 0;


};

