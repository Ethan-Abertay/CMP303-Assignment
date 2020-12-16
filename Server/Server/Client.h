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

	// Network info
	sf::IpAddress address;
	unsigned short port;
	unsigned int ID;

	// Game info
	deque<InfoPacket>* infoPackets;
};
