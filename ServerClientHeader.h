
// This header file is accessed by both the client and the server
// Below is the IP and port of the local host server
#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 4444

// Ping is found by pinging a different port so it can respond as quickly as possible
#define DEFAULT_PING_PORT 5555

// Tick rates (number of ticks per second)
#define SERVER_TICK_RATE 2
#define CLIENT_TICK_RATE 2

#include <string>

// Using declerations
using std::string;

// Global variables
const int maxMessageSize = sf::UdpSocket::MaxDatagramSize;	// The maximum number of bytes that can be sent in one message

enum class MessageType
{
	RequestJoin,
	JoinAccept,
	UpdateInfo,
	Disconnect,
	Ping
};

template<class T>
struct Vector3
{
	Vector3() {};
	Vector3(T a, T b, T c)
	{
		x = a;
		y = b;
		z = c;
	};

	T	x = 0, 
		y = 0,
		z = 0;
};

// At the beginning of every message to identify which message type was sent
struct MessageHeader
{
	int messageSize;
	MessageType messageType;
};

// The game information passed between clients and stored by the server
struct InfoPacket
{
	Vector3<float> position;
	float time;
};

// Used to update a client's position
struct UpdateInfoMessage
{
	MessageHeader header;
	InfoPacket infoPacket;
	unsigned int ID;
};

// Used by the server to tell the client that they have successfully joined and give them their unique client ID
struct JoinAcceptMessage
{
	MessageHeader header;
	unsigned int ID;			// The clients ID
	unsigned short pingPort;	// The port of the socket to ping
	float sentTime;				// The time at which this message was sent
};

struct PingMessage
{
	MessageType type;
};
