#include "Client.h"

Client::Client() :
	port(0),
	ID(0)
{
}

Client::Client(sf::IpAddress addrs, unsigned short port_param, unsigned int ID_param)
{
	address = addrs;
	port = port_param;
	ID = ID_param;

	// Initialise info packets deque
	infoPackets = new deque<InfoPacket>;
	/* For now, do this so it's easy to pop front */ infoPackets->push_back(InfoPacket());
}

Client::~Client()
{
	// Delete info packets
	delete infoPackets;
}

void Client::newInfoPacket(InfoPacket& info)
{
	// Push to back
	infoPackets->push_back(info);

	// Pop off front for now
	infoPackets->pop_front();
}
