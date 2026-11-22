#include "server.hpp"

// server.cpp

int server_init(int port, std::string passwd)
{
	// SERVER INIT
	
	// first create a peer socket/endpoint for communication
	// AF_INET for IPv4; 
	// SOCK_STREAM for TCP connection protocol; 
	// 0 for standard SOCK_STREAM;)
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	(void)passwd;

	if (sock_fd < 0)
	{
		std::cerr << "! socket error !" << std::endl;
		return (0);
	}

	// allow immediate port reuse after restart!
	int reuse = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	// socket needs to get an address, a place to be accessed
	// IPv4 localhost, in this case
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = INADDR_ANY; // in any address, just like 0.0.0.0
	address.sin_port = htons(port); // converts unsigned short int from host byte order to network byte order
	
	if (bind(sock_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
	{
		std::cerr << "! bind error !" << std::endl;
		close(sock_fd);
		return (0);
	}

	int backlog = 10; 	// backlog -> max number of connections 
						// (can't be bigger than 4096, as described in `/proc/sys/net/core/somaxconn`
						// new connections stay in line, queued
						// if connections queue is full, may get an error (ECONNREFUSED)
						// or they can be retransmitted and later try to connect again

	// listen() will make it a passive socket
	// it will be used to accept incoming connections using accept() later on
	if (listen(sock_fd, backlog) < 0)
	{
		std::cerr << "! listen error !" << std::endl;
		close(sock_fd);
		return (0);
	}

	std::cout << "peer socket fd " << sock_fd << " listening on :" << port << std::endl;
	return sock_fd;
}

