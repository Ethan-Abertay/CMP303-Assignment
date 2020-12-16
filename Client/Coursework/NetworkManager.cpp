#include "NetworkManager.h"

NetworkManager::NetworkManager(Player* player_ptr)
{
	// Initialise variables
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

	if (bAdjustingTime)
	{
		if (!bSentPing)
		{
			// Need to send ping
			sendPing();
		}
		else
		{
			// Need to receive ping

			// Decrease timer
			pingTimer -= dt;

			if (pingTimer > 0)
			{
				// Try to receive ping message
				float ping = receivePing();	// Returns the ping in seconds

				if (ping >= 0)			// If a ping was received
				{
					// Handle ping
					pingSum += ping;	// Add this ping to the sum

					// Handle counter
					currentPingNo++;

					// Determine if finished pinging
					if (currentPingNo >= noOfTimesToPing)
					{
						// Make adjustments to current time
						ping = pingSum / noOfTimesToPing;	// Get average
						ping /= 2.f;						// Divide by two
						currentTime -= ping;				// Subtract by value

						// End adjusting time phase
						bAdjustingTime = false;
						bConnected = true;
					}
				}
			}
			else
			{
				// Send ping again
				sendPing();
			}
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

		// Check to see if there's any packets to update the enemies with
		receiveUpdateInfo();
	}
}

void NetworkManager::newServer(string newIP, uint32_t newPort)
{
	// Check if attempting to connect to currently connected server
	if (newIP == serverIP && newPort == port)
		return;

	serverIP = newIP;
	port = newPort;

	sendJoinRequestMessage();
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
	sf::Socket::Status status = socket.receive((char*)&msg_buffer, maxMessageSize, receivedSize, sendersIP, sendersPort);

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
				bAdjustingTime = true;					// Time to get average ping and adjust current time
				bSentPing = false;						// Haven't sent ping yet
				ID = msg_full->ID;						// Set client's ID
				pingPort = msg_full->pingPort;			// Set the ping port
				currentTime = msg_full->sentTime;		// The the current time

				// Reset ping averaging variables
				pingSum = 0;
				currentPingNo = 0;
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
	pingSentTime = currentTime;

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

float NetworkManager::receivePing()
{
	size_t receivedSize;
	sf::IpAddress sendersIP;
	unsigned short sendersPort;
	sf::Socket::Status status = socket.receive((char*)&msg_buffer, maxMessageSize, receivedSize, sendersIP, sendersPort);

	// If received successfully
	if (status == sf::Socket::Done)
	{
		// Check this is a ping message
		PingMessage* msg_full = (PingMessage*)&msg_buffer;
		if (msg_full->type == MessageType::Ping)
		{
			// Reset boolean
			bSentPing = false;

			// Calculate ping
			float receivedTime = currentTime;
			return receivedTime - pingSentTime;
		}
	}
	else
	{
		return -1.f;
	}

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
	msg.ID = ID;

	// Send message
	if (socket.send((char*)&msg, msg.header.messageSize, serverIP, port) != sf::Socket::Done)
	{
		// Failed to send message
		assert(false);
	}
}

void NetworkManager::receiveUpdateInfo()
{

}

Vector3<float> NetworkManager::XMToVec3(XMFLOAT3 f3)
{
	return Vector3<float>(f3.x, f3.y, f3.z);
}
