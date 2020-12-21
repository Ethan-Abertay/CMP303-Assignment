#include "NetworkManager.h"
#include "App1.h"

NetworkManager::NetworkManager(App1* app1_ptr,  Player* player_ptr)
{
	// Initialise variables
	app1 = app1_ptr;
	player = player_ptr;			// Set player
	tickDelayTimer = tickDelayMax;	// Set timer to max

	// Set network specific variables
	resetMsgBuffer();			// Set msg buffer to all 0
	socket.setBlocking(false);	// Disable blocking

	// Setup socket
	sf::UdpSocket socket;	// Create SFML UDP socket class
	if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done)	// Bind this socket to any port
	{
		// Failed to bind socket
		assert(false);
	}
}

NetworkManager::~NetworkManager()
{
	delete pingThread;
}

void NetworkManager::frame(float dt)
{
	// Handle time
	currentTime += dt;

	if (bWaitingToEstablishConnection)
	{
		// Decrease timer
		establishConnectionTimer -= dt;

		if (establishConnectionTimer <= 0.f)
		{
			// Assume request didn't make it to server
			// Resence the request message
			sendJoinRequestMessage();
		}
		else
		{
			// Check to see if message was received
			receiveJoinAcceptMessage();
		}
	}
	
	if (bConnected)
	{
		// Decrease timer
		tickDelayTimer -= dt;

		// Check if time is up
		if (tickDelayTimer <= 0.f)
		{
			// It is time to update the server with this client's latest info
			sendUpdateInfo();
		}

		// Check to see any packets
		receiveMessages();

		// Manage enemies
		manageEnemies();
	}
}

void NetworkManager::newServer(string newIP, uint32_t newPort)
{
	// Check if attempting to connect to currently connected server
	if (newIP == serverIP && newPort == port)
		return;

	// Return is variables are null
	if (newIP == "" || newPort == 0)
		return;

	serverIP = newIP;
	port = newPort;

	sendJoinRequestMessage();
}

void NetworkManager::disconnect()
{
	// Return is already disconnected
	if (serverIP == "")
		return;

	// Tell server this client is disconnecting
	// If this packet is lost this client will timeout on server and clients
	ClientDisconnectMessage msg;
	msg.header.messageType = MessageType::ClientDisconnect;
	msg.header.messageSize = sizeof(ClientDisconnectMessage);
	msg.ID = ID;
	socket.send((char*)&msg, msg.header.messageSize, serverIP, port);

	// Delete all enemies
	app1->deleteAllEnemies();
	enemies.clear();

	// Reset variables
	serverIP = "";
	port = 0;
	bConnected = false;
}

void NetworkManager::shoot(XMFLOAT3 position, XMFLOAT3 forward)
{
	if (player->canFire())
	{
		// Construct message
		ProjectileShotMessage msg;
		msg.header.messageType = MessageType::ProjectileShot;
		msg.header.messageSize = sizeof(ProjectileShotMessage);
		msg.infoPacket.position = XMToVec3(position);
		msg.infoPacket.velocity = XMToVec3(player->getFireVelocity(forward));
		msg.infoPacket.time = currentTime;
		msg.ID = ID;

		// Send message
		socket.send((char*)&msg, msg.header.messageSize, serverIP, port);

		// Spawn projectile
		spawnProjectile(msg.infoPacket);
	}
}

void NetworkManager::receiveMessages()
{
	size_t receivedSize;
	sf::IpAddress sendersIP;
	unsigned short sendersPort;

	auto handleUpdateInfo = [&](UpdateInfoMessage* msg)
	{
		if (msg->entity == Entity::Client)	// If this is a client
		{
			// Create pointer
			Enemy* enemy;

			// Check if this enemy doesn't currently exist in our world
			if (!keyExists(enemies, msg->ID))
			{
				// Create new enemy
				enemy = app1->createEnemy();

				// Add enemy to map
				enemies[msg->ID] = enemy;
			}
			else
			{
				// Get enemy from map
				enemy = enemies[msg->ID];
			}

			// Update enemy
			enemy->newInfoUpdate(msg->infoPacket);
		}
		else if (msg->entity == Entity::Projectile)	// If this is a projectile
		{

		}
	};
	auto handleClientDisconnect = [&](ClientDisconnectMessage * msg)
	{
		Enemy* enemy = enemies[msg->ID];
		if (enemy)
		{
			app1->deleteEnemy(enemy);
			enemies.erase(msg->ID);
		}
	};
	auto handleProjectileShot = [&](ProjectileShotMessage* msg)
	{
		spawnProjectile(msg->infoPacket);
	};

	while (true)
	{
		sf::Socket::Status status = socket.receive((char*)&msg_buffer, PACKET_LIMIT, receivedSize, sendersIP, sendersPort);

		if (status == sf::Socket::Done)
		{
			// Ensure the message has been received from the correct place
			if (sendersIP != serverIP || sendersPort != port)
			{
				// Clear message buffer and continue
				resetMsgBuffer(receivedSize);
				continue;
			}

			// Convert message into struct
			MessageHeader* msg_header = (MessageHeader*)&msg_buffer;

			switch (msg_header->messageType)
			{
			case MessageType::UpdateInfo:
			{
				UpdateInfoMessage* msg_full = (UpdateInfoMessage*)&msg_buffer;
				handleUpdateInfo(msg_full);
				break;
			}
			case MessageType::ClientDisconnect:
			{
				ClientDisconnectMessage* msg_full = (ClientDisconnectMessage*)&msg_buffer;
				handleClientDisconnect(msg_full);
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

void NetworkManager::sendJoinRequestMessage()
{
	// Update variables to expect a responce
	bWaitingToEstablishConnection = true;
	establishConnectionTimer = establishConnectionTimeout;
	bConnected = false;

	// Build request message
	MessageHeader msg;
	msg.messageType = MessageType::RequestJoin;
	msg.messageSize = sizeof(MessageHeader);

	// Send request 
	if (socket.send((char*)&msg, msg.messageSize, serverIP, port) != sf::Socket::Done)
	{
		// Failed to send message
		assert(false);
	}
}

void NetworkManager::receiveJoinAcceptMessage()
{
	size_t receivedSize;
	sf::IpAddress sendersIP;
	unsigned short sendersPort;
	sf::Socket::Status status = socket.receive((char*)&msg_buffer, PACKET_LIMIT, receivedSize, sendersIP, sendersPort);

	// Ensure the message has been received from the correct place
	if (sendersIP != serverIP || sendersPort != port)
	{
		// Clear message buffer and return
		resetMsgBuffer(receivedSize);
		return;
	}

	if (status == sf::Socket::Done)
	{
		// Convert message into struct
		MessageHeader* msg_header = (MessageHeader*)&msg_buffer;

		// Ensure file sizes match
		if (!msg_header || receivedSize != msg_header->messageSize)
		{
			// Failed to receive message properly
			// Clear message buffer and return
			resetMsgBuffer();
			return;
		}

		// Ensure message is the expected type
		if (msg_header->messageType == MessageType::JoinAccept)
		{
			// Convert to correct message type
			JoinAcceptMessage* msg_full = (JoinAcceptMessage*)&msg_buffer;

			if (msg_full)
			{
				// Acceptance successfully received
				// Set variables and just exit
				bWaitingToEstablishConnection = false;	// No longer waiting to establish a connection
				bConnected = false;						// Ensure false so only the ping thread can use the socket
				bSentPing = false;						// Haven't sent ping yet
				ID = msg_full->ID;						// Set client's ID
				pingPort = msg_full->pingPort;			// Set the ping port
				currentTime = msg_full->sentTime;		// The the current time

				// Reset ping averaging variables
				pingSum = 0;
				currentPingNo = 0;

				// Dispatch the ping thread
				if (pingThread)
				{
					pingThread->join();
					delete pingThread;
				}
				pingThread = new thread(&NetworkManager::ping, this);
			}
		}
		else
		{
			// Wrong message received
			// Clear message buffer and return
			resetMsgBuffer();
			return;
		}
	}
	else if (status == sf::Socket::Partial)
	{
		// Handle this if this scenario arrises 
		assert(false);
	}
}

void NetworkManager::sendPing()
{
	// Set variables
	pingTimer = pingTimeout;
	bSentPing = true;

	// Build request message
	PingMessage msg;
	msg.type = MessageType::Ping;

	// Send request 
	if (socket.send((char*)&msg, sizeof(PingMessage), serverIP, pingPort) != sf::Socket::Done)
	{
		// Failed to send message
		assert(false);
	}
}

bool NetworkManager::receivePing()
{
	size_t receivedSize;
	sf::IpAddress sendersIP;
	unsigned short sendersPort;
	sf::Socket::Status status = socket.receive((char*)&msg_buffer, PACKET_LIMIT, receivedSize, sendersIP, sendersPort);

	// If received successfully
	if (status == sf::Socket::Done)
	{
		// Check this is a ping message
		PingMessage* msg_full = (PingMessage*)&msg_buffer;
		if (msg_full->type == MessageType::Ping)
		{
			// Reset boolean
			bSentPing = false;

			return true;
		}
	}

	return false;
}

void NetworkManager::sendUpdateInfo()
{
	// Reset timer
	tickDelayTimer = tickDelayMax;

	// Construct message
	UpdateInfoMessage msg;
	msg.header.messageType = MessageType::UpdateInfo;
	msg.header.messageSize = sizeof(UpdateInfoMessage);
	msg.infoPacket.position = XMToVec3(player->getPosition());
	msg.infoPacket.time = currentTime;
	msg.entity = Entity::Client;
	msg.ID = ID;

	// Send message
	if (socket.send((char*)&msg, msg.header.messageSize, serverIP, port) != sf::Socket::Done)
	{
		// Failed to send message
		assert(false);
	}
}

Vector3<float> NetworkManager::XMToVec3(XMFLOAT3 f3)
{
	return Vector3<float>(f3.x, f3.y, f3.z);
}

XMFLOAT3 NetworkManager::Vec3ToXM(Vector3<float> v3)
{
	return XMFLOAT3(v3.x, v3.y, v3.z);
}

void NetworkManager::ping()
{
	// Initialise time variables
	auto pingSentTime = chronoClock::now();
	auto pingArrivedTime = chronoClock::now();
	auto newLoopTime = chronoClock::now();
	auto oldLoopTime = chronoClock::now();
	float dt;	// Delta time in seconds

	while (true)
	{
		newLoopTime = chronoClock::now();	// Measure new loop time
		dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<float>(newLoopTime - oldLoopTime)).count() / 1000.f;	// Get duration since last loop
		oldLoopTime = newLoopTime;			// Set old loop time

		if (!bSentPing)
		{
			// Need to send ping
			sendPing();
			pingSentTime = chronoClock::now();
		}
		else
		{
			// Need to receive ping

			// Decrease timer
			pingTimer -= dt;

			if (pingTimer > 0)
			{
				// Try to receive ping message
				bool bGotPing = receivePing();	// Returns the ping in nanoseconds

				if (bGotPing)			// If a ping was received
				{
					// Handle ping
					pingArrivedTime = chronoClock::now();
					float ping = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<float>(pingArrivedTime - pingSentTime)).count();
					pingSum += ping;	// Add this ping to the sum

					// Handle counter
					currentPingNo++;

					// Determine if finished pinging
					if (currentPingNo >= noOfTimesToPing)
					{
						// Make adjustments to current time
						ping = pingSum / (float)noOfTimesToPing;	// Get average
						ping /= 2.f;						// Divide by two
						ping /= 1000.f * 1000.f;			// Convert to seconds from microseconds
						currentTime -= ping;				// Subtract by value

						// End ping calculations
						bConnected = true;
						break;
					}
				}
			}
			else
			{
				// Send ping again
				bSentPing = false;
			}
		}
	}
}

void NetworkManager::manageEnemies()
{
	vector<unsigned int> eraseKeys;

	// Iterate through enemies are mark them for deletion if they have timed out
	for (auto it = enemies.begin(); it != enemies.end(); it++)
	{
		if (it->second->timeSinceLastMessage >= TIMEOUT)
		{
			// Flag key for deletion
			eraseKeys.push_back(it->first);
		}
	}

	for (int i = 0; i < eraseKeys.size(); i++)
	{
		app1->deleteEnemy(enemies[eraseKeys[i]]);
		enemies.erase(eraseKeys[i]);
	}
}

void NetworkManager::spawnProjectile(ProjectileShotPacket& info)
{
	app1->newProjectile(Vec3ToXM(info.position), Vec3ToXM(info.velocity), PROJECTILE_TIMEOUT);
}

