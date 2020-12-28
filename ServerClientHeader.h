
// This header file is accessed by both the client and the server
// Below is the IP and port of the local host server
#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 5555

// Ping is found by pinging a different port so it can respond as quickly as possible
#define DEFAULT_PING_PORT 5556

// Tick rates (number of ticks per second)
#define SERVER_TICK_RATE 16
#define CLIENT_TICK_RATE 16

// Times (in seconds) for interp. and extrap.
#define INTERP_BUFFER_TICKS 3	// The number of ticks to delay the clients by to interpolate
#define EXTRAP_TICK_MAX 4		// The maximum number of ticks in a row that can be lost before extrapolation stops
#define INTERP_BUFFER_TIME 1.f / CLIENT_TICK_RATE * INTERP_BUFFER_TICKS		// The length of time of the buffer for the defined number of ticks 
#define EXTRAP_TIME_MAX 1.f / CLIENT_TICK_RATE * EXTRAP_TICK_MAX			// The extrap tick converted into time

#define PACKET_LIMIT sf::UdpSocket::MaxDatagramSize

#define PROJECTILE_TIMEOUT 5.f

#define TIMEOUT 10.f
#define PROJECTILE_MAX_RANGE 100.f

#include <string>

// Using declerations
using std::string;


enum class MessageType
{
	RequestJoin,
	JoinAccept,
	UpdateInfo,
	ClientDisconnect,
	Ping,
	ProjectileShot,
	ClientHit
};

enum class Entity
{
	Client,
	Projectile
};

template<class T>
struct Vector3
{
	// Constructors
	Vector3() {};
	Vector3(T a, T b, T c)
	{
		x = a;
		y = b;
		z = c;
	};

	// Operators
	Vector3<T> operator+(const Vector3<T> vec) const 
	{
		Vector3<T> output = *this;
		output.x += vec.x;
		output.y += vec.y;
		output.z += vec.z;
		return output;
	}
	Vector3<T> operator-(const Vector3<T> vec) const  
	{
		Vector3<T> output = *this;
		output.x -= vec.x;
		output.y -= vec.y;
		output.z -= vec.z;
		return output;
	}
	void operator+=(const Vector3<T> vec) 
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
	}
	Vector3<T> operator*(const Vector3<T> vec) const  
	{
		Vector3<T> output = *this;
		output.x *= vec.x;
		output.y *= vec.y;
		output.z *= vec.z;
		return output;
	}
	Vector3<T> operator*(const T val) const  
	{
		Vector3<T> output = *this;
		output.x *= val;
		output.y *= val;
		output.z *= val;
		return output;
	}
	
	// Static Functions
	static float length(const Vector3<T> vec) 
	{
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	// Member Functions
	Vector3<T> cross(const Vector3<T> vec) const
	{
		Vector3<T> output;
		output.x = y * vec.z - z * vec.y;
		output.y = z * vec.x - x * vec.z;
		output.z = x * vec.y - y * vec.x;
		return output;
	}
	float dot(const Vector3<T> vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}
	void normalized()
	{
		float magnitude = length(*this);
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}
	Vector3<T> normalize() const
	{
		float magnitude = length(*this);
		Vector3<T> vec = *this;
		vec.x /= magnitude;
		vec.y /= magnitude;
		vec.z /= magnitude;
		return vec;
	}
	float angle(const Vector3<T> vec) const
	{
		return acosf(dot(vec) / (length() * length(vec)));
	}
	float length() const
	{
		return length(*(this));
	}
	float sum() const
	{
		return x + y + z;
	}

	// Variables
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

// The game information passed by clients and stored by the server
struct ClientInfoPacket
{
	Vector3<float> position;
	float time;
};

// Sent by client to server to construct projectile
struct ProjectileShotPacket
{
	Vector3<float> position;
	Vector3<float> velocity;	// In m/s
	float time;					// Time the projectile was shot
};

// Used to update an entity's position
struct UpdateInfoMessage
{
	MessageHeader header;
	ClientInfoPacket infoPacket;
	Entity entity;
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

// Tells clients and servers that a client is disconnecting
struct ClientDisconnectMessage
{
	MessageHeader header;
	unsigned int ID;
};

// Sent by client to tell server that a projectile has been fired
struct ProjectileShotMessage
{
	MessageHeader header;
	ProjectileShotPacket infoPacket;
	unsigned int ID;	// ID of shooter
};

// Sent by server to tell client they have been hit
struct ClientHitMessage
{
	MessageHeader header;
};

struct PingMessage
{
	MessageType type;
};

// This class handles interpolation and extrapolation (prediction)
// This class is used by both the client and the server
class Interp
{
public:

	// Returns an info packet interpolated to the supplied time
	static ClientInfoPacket interpolate(ClientInfoPacket oldPacket, ClientInfoPacket newPacket, float time)
	{
		ClientInfoPacket output;
		output.time = time;
		
		// Get direction vector
		const Vector3<float> direction = newPacket.position - oldPacket.position;

		// Get percentage increase in time from the old to the new
		// Old = 0%, new = 100%. Although it is kept in decimal format
		const float timePercent = 1.f - (newPacket.time - time) / (newPacket.time - oldPacket.time);

		// Linearly interpolate the position
		output.position = oldPacket.position + (direction * timePercent);

		// Return
		return output;
	}
};
