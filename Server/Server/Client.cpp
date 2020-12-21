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
	infoPackets = new deque<ClientInfoPacket>;
}

Client::~Client()
{
	// Delete info packets
	if (infoPackets)
	{
		delete infoPackets;
		infoPackets = 0;
	}
}

void Client::newInfoPacket(ClientInfoPacket& info)
{
	// Push to back
	infoPackets->push_back(info);

	// Detect if oldest packet is over a second old, if so, delete it
	if (infoPackets->front().time + 1.f < infoPackets->back().time)
		infoPackets->pop_front();

	// It is not worth iterating through all packets every frame to check since this is only done once a new packet is added which will limit the overall size
}

ClientInfoPacket* Client::getLatestInfoPacket()
{
	if (infoPackets->size() > 0)
		return &infoPackets->back();
	return 0;
}

ClientInfoPacket* Client::getSecondLastInfoPacket()
{
	if (infoPackets->size() > 1)
		return &(*infoPackets)[infoPackets->size() - 2];
	return 0;
}
