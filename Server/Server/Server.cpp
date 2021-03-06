#include "Server.h"

Server::Server()
{
	socket.setBlocking(false);		// Disable blocking
	resetMsgBuffer();				// Clear msg buffer
	currentTime = 0.f;				// Set time to 0.f
	clock.restart();				// Restart the timer
}

Server::~Server()
{
	killThreads();

	// Delete clients
	for (auto& it : clients)
	{
		delete it.second;
		it.second = 0;
	}
	clients.clear();

	// Delete projectiles
	for (auto it = projectiles.begin(); it != projectiles.end(); it++)
	{
		delete *it;
		*it = 0;
	}
	projectiles.clear();
}

void Server::init(string serverIP, int port)
{
	// Initialise socket with provided parameters
	if (socket.bind(port, serverIP) != sf::Socket::Done)
	{
		// Socket failed to bind
		assert(false);
	}

	// Start up ping thread
	pingThread = new thread(&Server::ping, this, serverIP);

	run();
}

void Server::run()
{
	while (bExecuting)
	{
		// Handle time data
		dt = clock.restart().asSeconds();		// Restarts the clock and returns the elapased time
		currentTime += dt;

		printf("\nServer Tick:\n");	// Output tick message

		receiveMessages();			// Receive messages

		sendMessages();				// Send messages

		manageClients();			// Manage clients

		manageProjectiles();		// Manage projectiles

		// Determine how long to sleep to hit tick rate
		float delay = tickDelay - (float)clock.getElapsedTime().asMilliseconds();
		if (delay < 0.f)
			delay = 0.f;
		printf("Delay is %.3fms\n", delay);
		Sleep(delay);
	}
}

void Server::receiveMessages()
{
	// Info about received packet
	size_t receivedSize;
	sf::IpAddress sendersIP;
	unsigned short sendersPort;

	auto handleRequestJoin = [&](MessageHeader* msg)
	{
		// Ensure this socket doesn't already exist
		for (auto it = clients.begin(); it != clients.end(); it++)	// Iterate through unordered map with constant complexity O(1) per iteration
		{
			if (it->second)
			{
				if (it->second->address == sendersIP && it->second->port == sendersPort)
				{
					// This socket is already joined, must return 
					printf("Client is already on the server\nResending Join Accept Message...\n");
					sendJoinAcceptMessage(it->first);
					return;
				}
			}
		}
		// This is a new socket to add

		// Get this client a unique id
		unsigned int ID = 0;
		srand(0);
		do
		{
			ID = rand();	// 2^32 possible players per server maximum

			// Ensure this isn't a duplicated ID
			if (!keyExists(clients, ID))
				break;

		} while (true);

		// Add client to map
		clients[ID] = new Client(sendersIP, sendersPort, ID);

		// Send reply to client
		sendJoinAcceptMessage(ID);

		// Output info
		printf("Successfully added new client\n");
	};
	auto handleUpdateInfo = [&](UpdateInfoMessage* msg)
	{
		if (msg->entity == Entity::Client) // If this is a client
		{
			// Get the correct client
			Client* client = clients[msg->ID];	// This creates this index and sets second to null

			if (client)
			{
				// Set time since last message to 0
				client->timeSinceLastMessage = 0.f;

				// Add the new packet info on
				client->newInfoPacket(msg->infoPacket);

				printf("Received info packet from client: %i\n", msg->ID);
			}
			else
			{
				// Delete the empty second
				clients.erase(msg->ID);
			}
		}
	};
	auto handleClientDisconect = [&](ClientDisconnectMessage* msg)
	{		
		// Tell each client except this client that this client is disconnecting
		for (auto it = clients.begin(); it != clients.end(); it++)
		{
			if (it->first != msg->ID)	// Don't message same client that's disconnecting
			{
				socket.send((char*)&*msg, msg->header.messageSize, it->second->address, it->second->port);
			}
		}

		// Delete enemy
		delete clients[msg->ID];
		clients.erase(msg->ID);

		// Output
		printf("Client %i has disconnected\n", msg->ID);
	};
	auto handleProjectileShot = [&](ProjectileShotMessage* msg)
	{
		// Ensure this isn't a duplicated message
		if (projectiles.size() > 1 && projectiles.front()->getStartTime() == msg->infoPacket.time)
			return;

		// Add projectile to the list
		projectiles.push_front(new Projectile(&clients, msg->infoPacket, msg->ID));

		// Tell all clients (except sender) that this projectile now exists
		for (auto it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second->ID != msg->ID)
			{
				socket.send((char*)&*msg, msg->header.messageSize, it->second->address, it->second->port);
			}
		}

		// Output message
		printf("Projectile shot by client: %i at %f seconds\n", msg->ID, msg->infoPacket.time);
	};

	// The message buffer is always reset before calling receive
	assert(msg_buffer[0] == 0);

	while (true)
	{
		// Attempt to read from socket. 
		sf::Socket::Status status = socket.receive(&msg_buffer, PACKET_LIMIT, receivedSize, sendersIP, sendersPort);

		// Only process successfully received messages
		if (status == sf::Socket::Done)
		{
			// This works because the message header is always first in all messages and c++ structs are stored contiguously in memory thankfully
			MessageHeader* msg_header = (MessageHeader*)&msg_buffer;

			// Ensure file sizes match
			if (!msg_header || receivedSize != msg_header->messageSize)
			{
				// Failed to receive message properly
				resetMsgBuffer();
				continue;
			}

			// Determine and convert to correct message type
			switch (msg_header->messageType)
			{
			case MessageType::RequestJoin:
			{
				handleRequestJoin(msg_header);
				break;
			}
			case MessageType::UpdateInfo:
			{
				UpdateInfoMessage* msg_full = (UpdateInfoMessage*)&msg_buffer;
				handleUpdateInfo(msg_full);
				break;
			}
			case MessageType::ClientDisconnect:
			{
				ClientDisconnectMessage* msg_full = (ClientDisconnectMessage*)&msg_buffer;
				handleClientDisconect(msg_full);
				break;
			}
			case MessageType::ProjectileShot:
			{
				ProjectileShotMessage* msg_full = (ProjectileShotMessage*)&msg_buffer;
				handleProjectileShot(msg_full);
				break;
			}
			}

			// Clear the message buffer using the exact size to increase efficiency
			resetMsgBuffer(receivedSize);
		}
		else if (status == sf::Socket::NotReady)
		{
			// There are no more messages to read and we can break
			break;
		}
		else
		{
			// If the message was not successful and not NotReady than it was either partially sent or an error
			// Reset the message buffer and continue
			resetMsgBuffer();
		}
	}
}

void Server::sendMessages()
{
	// This is the method that sends a single packet per client info
	// This results in a lot of packets being sent which has a greater overhead
	// But it means a packet loss will only affect one enemy instead of them all
	// Using one packet containing all client info benefits from less overhead but is more noticable during packet loss

	// Update each client with all other clients's latest information
	for (auto it = clients.begin(); it != clients.end(); it++)
	{
		// Continue if client is null
		if (!it->second)
			continue;

		for (auto it2 = clients.begin(); it2 != clients.end(); it2++)
		{
			// Continue if client is null
			if (!it2->second)
				continue;

			// Don't send clients info about themselves
			if (it != it2)
			{
				sendUpdateInfo(it2->second, it->second);
			}
		}

		// Detect if the client has been hit
		if (it->second->bHit)
		{
			// Set flag false
			it->second->bHit = false;
			
			// Send message
			ClientHitMessage msg;
			msg.header.messageType = MessageType::ClientHit;
			msg.header.messageSize = sizeof(ClientHitMessage);
			
			socket.send((char*)&msg, msg.header.messageSize, it->second->address, it->second->port);
		}
	}
}

void Server::manageClients()
{
	vector<unsigned int> eraseKeys;

	// Increment each client's timer and check to see if any have timed out
	for (auto it = clients.begin(); it != clients.end(); it++)	// Iterate through map
	{
		if (it->second)
		{
			// Increment timer
			it->second->timeSinceLastMessage += dt;

			// Check for time out
			if (it->second->timeSinceLastMessage >= TIMEOUT)
			{
				// Flag for removal
				eraseKeys.push_back(it->first);
			}
		}
	}

	// Erase flagged clients
	for (int i = 0; i < eraseKeys.size(); i++)
	{
		// Construct disconnect message
		ClientDisconnectMessage msg;
		msg.header.messageType = MessageType::ClientDisconnect;
		msg.header.messageSize = sizeof(ClientDisconnectMessage);
		msg.ID = eraseKeys[i];

		// Tell all other clients that this client has disconnected
		// If this packet is lost, the disconnected client will timeout for the other client anyway
		for (auto it = clients.begin(); it != clients.end(); it++)
		{
			if (it->second)
			{
				// Don't send message to a client about to a disconnected client
				bool bContinue = false;
				for (int j = 0; j < eraseKeys.size(); j++)
				{
					if (it->first == eraseKeys[j])	// If this client is a disconnected client
					{
						bContinue = true;
						break;
					}
				}
				if (bContinue)
					continue;

				// Send message to this client about the disconnected client
				socket.send((char*)&msg, msg.header.messageSize, it->second->address, it->second->port);
			}
		}

		// Delete client
		delete clients[eraseKeys[i]];
		clients.erase(eraseKeys[i]);
	}
}

void Server::manageProjectiles()
{
	// Update each projectile and delete if frame returns false
	for (auto it = projectiles.begin(); it != projectiles.end();)
	{
		if (!(*it)->frame(dt, currentTime))
		{
			it = projectiles.erase(it);
		}
		else
			it++;
	}
}

void Server::sendJoinAcceptMessage(int ID)
{
	JoinAcceptMessage msg;
	msg.header.messageType = MessageType::JoinAccept;
	msg.header.messageSize = sizeof(JoinAcceptMessage);
	msg.ID = ID;
	msg.pingPort = DEFAULT_PING_PORT;
	msg.sentTime = currentTime;

	// Attempt to send 5 times, then give up
	for (int i = 0; i < 5; i++)
	{
		if (socket.send((char*)&msg, msg.header.messageSize, clients[ID]->address, clients[ID]->port) == sf::Socket::Done)
		{
			// Message was successfully sent
			break;
		}
	}
}

void Server::sendUpdateInfo(Client* enemy, Client* player)
{
	// Check the enemy currently has info to send
	auto* packet_ptr = enemy->getLatestInfoPacket();
	if (!packet_ptr)
		return;

	// Check we are not sending the same message twice unnecessarily 
	if (packet_ptr->time == player->getLatestPacketTime(enemy->ID))
		return;

	UpdateInfoMessage msg;
	msg.header.messageType = MessageType::UpdateInfo;
	msg.header.messageSize = sizeof(UpdateInfoMessage);
	msg.entity = Entity::Client;
	msg.ID = enemy->ID;
	msg.infoPacket = *packet_ptr;

	// Attempt to send message
	socket.send((char*)&msg, msg.header.messageSize, player->address, player->port);

	// Update latest message time
	player->setLatestPacketTime(enemy->ID, msg.infoPacket.time);

	printf("Sending info about client: %i, to client: %i\n", enemy->ID, player->ID);
}

template<class A, class B>
bool Server::keyExists(unordered_map<A, B> &map_, A key)
{
	if (map_.find(key) == map_.end())
		return false;
	return true;
}

void Server::ping(string serverIP)
{
	// Setup socket
	sf::UdpSocket pingSocket;
	pingSocket.setBlocking(false);	// Set blocking false
	if (pingSocket.bind(DEFAULT_PING_PORT, serverIP) != sf::Socket::Done)	// Bind socket
	{
		// Socket failed to bind
		assert(false);
	}

	// Setup selector
	sf::SocketSelector selector;
	selector.add(pingSocket);

	// Sender's data
	size_t receivedSize;
	sf::IpAddress sendersIP;
	unsigned short sendersPort;

	// Buffer
	char buffer[sizeof(PingMessage)];
	memset(&buffer, 0, sizeof(PingMessage));

	while (bExecuting)
	{
		if (selector.wait(sf::seconds(10.f)))
		{
			// Receive message
			if (pingSocket.receive((char*)&buffer, sizeof(PingMessage), receivedSize, sendersIP, sendersPort) == sf::Socket::Done)
			{
				// Ensure this is a ping message 
				PingMessage* msg = (PingMessage*)&buffer;
				if (msg->type == MessageType::Ping)
				{
					// Send reply
					pingSocket.send((char*)&buffer, sizeof(PingMessage), sendersIP, sendersPort);

					// Output message
					printf("Server Pinged\n");
				}

				memset(&buffer, 0, sizeof(PingMessage));
			}
		}
	}
}

void Server::killThreads()
{
	if (bExecuting)
	{
		bExecuting = false;	// Causes execution to stop on all threads

		if (pingThread)
		{
			pingThread->join();	// Wait to finish executing
			delete pingThread;
			pingThread = 0;
		}
	}
}
