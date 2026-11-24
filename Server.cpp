#include "Server.hpp"

// server.cpp

Server::Server(): _pass(""), _port(0), _sockfd(-1) {}

void Server::init(int port, std::string passwd)
{
	_port = port;
	_pass = passwd;
	
	std::cout << "port: " << _port << std::endl;
	std::cout << "pass: " << _pass << std::endl;
	
	// SERVER INIT
	
	// first create a peer socket/endpoint for communication
	// AF_INET for IPv4; 
	// SOCK_STREAM for TCP connection protocol; 
	// 0 for standard SOCK_STREAM;)
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	(void)passwd;

	if (_sockfd < 0)
	{
		std::cerr << "! socket error !" << std::endl;
		exit (1);
	}

	// allow immediate port reuse after restart!
	int reuse = 1;
	setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	// socket needs to get an address, a place to be accessed
	// IPv4 localhost, in this case
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = INADDR_ANY; // in any address, just like 0.0.0.0
	address.sin_port = htons(port); // converts unsigned short int from host byte order to network byte order
	
	if (bind(_sockfd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
	{
		std::cerr << "! bind error !" << std::endl;
		close(_sockfd);
		exit (1);
	}

	int backlog = 10; 	// backlog -> max number of connections 
						// (can't be bigger than 4096, as described in `/proc/sys/net/core/somaxconn`
						// new connections stay in line, queued
						// if connections queue is full, may get an error (ECONNREFUSED)
						// or they can be retransmitted and later try to connect again

	// listen() will make it a passive socket
	// it will be used to accept incoming connections using accept() later on
	if (listen(_sockfd, backlog) < 0)
	{
		std::cerr << "! listen error !" << std::endl;
		close(_sockfd);
		exit (1);
	}
}

void Server::loop()
{
	// define the master set, and then clean it
	fd_set master;
	FD_ZERO(&master);

	// add the listening sock to the master set
	FD_SET(_sockfd, &master);

	int maxFd = _sockfd;
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
		// - recv() message from client, and pass to another client - pass_message() ?
		
		if (FD_ISSET(_sockfd, &master_dup)) // new connection
		{
			int cfd = accept(_sockfd, NULL, NULL);
			if (cfd >= 0)
			{
				// TODO - A NEW OBJECT OF CLASS CLIENTS NEEDS TO BE CREATED/INSTANTIATED HERE!
				// this client will receive his cfd, this cfd will identify him inside the backend
				// client needs _user, _nick and, _cfd
				

				FD_SET(cfd, &master);
				clients.push_back(cfd);
				if (cfd > maxFd)
					maxFd = cfd; // here number of clients get's updated


				// TODO - create a `welcometochat()` message, from server to all clients (for debugging!)
				std::string welcome = "Welcome to Chat!\n";
				send(cfd, welcome.c_str(), welcome.size(), 0);

				std::cout << "client connected to fd: " << cfd << std::endl;
				std::cout << "number of clients connected are: " << clients.size() << std::endl;
				std::cout << "- - - - -" << std::endl;
			}
		}

		// check existing clients for incoming messages or disconnects
		for (size_t i = 0; i < clients.size(); )
		{
			int sock = clients[i];
			bool removed = false;

			// if this sockfd is in the master set, so it want to send messages
			if (FD_ISSET(sock, &master_dup))
			{
				// TODO - create a pass_message() to client
				char buf[BUFF_SIZE];
				memset(buf, 0, sizeof(buf));

				ssize_t bytesIn = recv(sock, buf, sizeof(buf) - 1, 0);
				// std::cout << "number of bytesIn read: " << bytesIn << std::endl;
				if (bytesIn <= 0)
				{
					// disconnect the client
					std::cout << "disconnecting client because bytesIn <= 0" << std::endl;
					close(sock);
					FD_CLR(sock, &master);
					clients.erase(clients.begin() + i);
					removed = true;
				}
				else
				{
					std::string input(buf, bytesIn);
					
					// while (!input.empty() && (input[input.size() - 1] == '\n' || input[input.size() - 1] == '\r'))
					// {
					// 	input.erase(input.size() - 1);
					// }
					
					// std::cout << "input.length() after striping the end: " << input.length() << std::endl;

					// check if it's quit cmd
					if (!input.empty() && input.length() == 5 && !strncmp(input.c_str(), ":q!", 3))
					{
						close(sock);
						FD_CLR(sock, &master);
						clients.erase(clients.begin() + i);
						removed = true;
						std::cout << "client " << sock << " has left the chat" << std::endl;
						std::cout << "- - - - -" << std::endl;

						// running = false;
						// break;
					}


					// otherwise, send message to everyone
					else
					{
						// std::cout << "should print a message" << std::endl;
						// std::cout << "clients.size() = " << clients.size() << std::endl;
						// std::cout << "clients[0] = " << clients[0] << std::endl;

						// char msg[bytesIn];
						// strcpy(msg, input.c_str());

						std::ostringstream ss;

						// ss << "SOCKET #" << sock << " says: " << input << "\r\n" ;
						ss << "SOCKET #" << sock << " says: " << input ;

						std::string output = ss.str();

						size_t len = output.size();
						char msg[len];
						strcpy(msg, output.c_str());
						
						// message to server
						std::cout << output;
						std::cout << "- - - - -" << std::endl;

						for (long unsigned int i = 0; i < clients.size(); i++)
						{
							// message to clients
							if (clients[i] != sock)
								send(clients[i], msg, sizeof(msg), 0);
						}
					}
				}
			}
			if (!removed)
				i++;
		}
	}

	// stop accepting new conns
	FD_CLR(_sockfd, &master);
	close(_sockfd);

	// send goodbye message to clients and close sockets
	std::string goodbye = "Server is shutting down. Goodbye\r\n";
	for (long unsigned sock = 0; sock < clients.size(); sock++ )
	{
		send(sock, goodbye.c_str(), goodbye.size(), MSG_NOSIGNAL);
		FD_CLR(sock, &master);
		close(sock);
	}
	clients.clear();
}


std::string Server::get_pass() { return _pass; } 
int			Server::get_port() { return _port; }
int			Server::get_sockfd() { return _sockfd; }
