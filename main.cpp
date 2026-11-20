#include <asm-generic/socket.h>
#include <cstdio>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <cstdlib>
#include <vector>

#define BUFF_SIZE 4096

// to be done: SERVER
// class Server
// {
// 	private:
// 		std::string _pass;
// 		int _port;

// };
// class User
// {
// }

int main(int ac, char *av[])
{
	// VERY BASIC, POLITE AND TREMENDOUSLY INACCURATE PARSING
	if (ac != 3)
	{
		std::cerr << "only 3 args accepted" << std::endl;
		std::cout << "./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port = atoi(av[1]); // make a parser for the port
							// having in consideration: 
							// - ports below 1024 are privileged/reserved ports
							// - max int port is 65xxx

	std::cout << "server will try port :" <<  port << std::endl;

	// SERVER INIT
	
	// first create a peer socket/endpoint for communication
	// AF_INET for IPv4; 
	// SOCK_STREAM for TCP connection protocol; 
	// 0 for standard SOCK_STREAM;)
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd < 0)
	{
		std::cerr << "! socket error !" << std::endl;
		exit(1);
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
		exit(1);
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
		exit(1);
	}

	std::cout << "peer socket fd " << sock_fd << " listening on :" << port << std::endl;
	
	// define the master set, and then clean it
	fd_set master;
	FD_ZERO(&master);

	// add the listening sock to the master set
	FD_SET(sock_fd, &master);

	int maxFd = sock_fd;
	std::vector<int> clients;
	bool running = true;

	// create a running server that handles multiple conns
	while(running)
	{
		// make a copy of the master set!
		// because everytime you call select(2), the set gets destroyed (we need to prevent the loss)
		fd_set master_dup = master;

		int socketCount = select(maxFd + 1, &master_dup, NULL, NULL, NULL); // NULL instead of nullptr
		if (socketCount < 0)
		{
			std::cerr << "! select() failed !" << std::endl;
			break ;
		}
		
		// the only 2 things sockets can do (regarding incoming data): 
		// - accept() connections/requests from clients
		// - recv() message from client
		
		// if socket at position i is a listening socket, that's an inbound communication
		// accept a new conection
		// add the new conn to the list of conn clients
		// send a welcome message to conn client
		if (FD_ISSET(sock_fd, &master_dup))
		{
			int cfd = accept(sock_fd, NULL, NULL);
			if (cfd >= 0)
			{
				FD_SET(cfd, &master);
				clients.push_back(cfd);
				if (cfd > maxFd)
					maxFd = cfd;

				std::string welcome = "Welcome to Chat!\n";
				send(cfd, welcome.c_str(), welcome.size(), 0);

				std::cout << "number of clients connected are: " << clients.size() << std::endl;
			}
		}



		// check existing clients for incoming messages or disconnects

	}



	return 0;
}


