#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <cstdlib>

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
	
	bool isExit = false; // needed to get out of the loop

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

	// socket needs to get an address, a place to be accessed
	// IPv4 localhost, in this case
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = INADDR_ANY; // in any address, just like 0.0.0.0
	address.sin_port = htons(port); // converts unsigned short int from host byte order to network byte order
									  // which will make it something like 00000000.00000000.00000000.00000000
	
	if (bind(sock_fd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
	{
		std::cerr << "! bind error !" << std::endl;
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
		exit(1);
	}

	std::cout << "peer socket fd " << sock_fd << " listening on :" << port << std::endl;
	
	// next fnct might needed later on: 
	// - to transform the size of the address into a socketlen_t type
	// - so it can be stored direclty in the struct/object
	// socklen_t addr_size = sizeof(address);
	
	int client = accept(sock_fd, NULL, NULL);
	if (client < 0)
	{
		std::cerr << "! accept error !" << std::endl;
		close(sock_fd);
		return 1;
	}
	std::cout << "socket client " << client << " accepted" << std::endl;
	
	char confirmation[256] = "you've entered the IRC server\n"
							"you can now send messages to the server!\n";

	send(client, confirmation, strlen(confirmation), 0);

	char message[BUFF_SIZE];
	memset(message, 0, BUFF_SIZE);

	while (isExit != true)
	{
		memset(message, 0, BUFF_SIZE);
		recv(client, message, BUFF_SIZE-1, 0);

		if (strlen(message) == 5 &&  !strncmp(message, ":q!", 3))
		{
			isExit = true;
			std::cout << "Client asked to leave." << std::endl;
			break ;
		}

		// for DEBUG
		// recv() gets a value that's always 2 bigger than "real strlen"
		// (probably because of 1 newline character and 1 null character?)
		std::cout << "strlen(message): " << strlen(message) << std::endl;

		std::cout << message << std::endl;
	}

	char shutdown[256] = "Server is about to close.\n"
						"See you next time!\n";

	send(client, shutdown, strlen(shutdown), 0);
	std::cout << "Closing socket." << std::endl;
	close(client);

	return 0;
}


