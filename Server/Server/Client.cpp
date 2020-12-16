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
}

Client::~Client()
{
	// Delete info packets
	delete infoPackets;
}
