
#include "Server.h"

#include <thread>

using std::thread;

int main()
{
	// Set up with threads to demonstrate how this could run multiple different server processes on different threads
	// This program only runs one server however

	const int noOfServers = 1;
	thread *threads[noOfServers];
	Server servers[noOfServers];

	// Start the server processes
	for (int i = 0; i < noOfServers; i++)
	{
		threads[i] = new thread(&Server::init, &servers[i], LOCAL_IP, LOCAL_PORT);
	}

	// Wait for all threads to join
	for (int i = 0; i < noOfServers; i++)
	{
		threads[i]->join();
	}

	// Delete all threads
	delete[] threads;

	return 0;
}