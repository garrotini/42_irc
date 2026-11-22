#include "server.hpp"

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
//

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
	
	std::string passwd = av[2];

	std::cout << "server will try port :" <<  port << std::endl;

	// PARSER WILL DELIVER: int=port & string=passwd

	int sock_fd = server_init(port, passwd);

	if(sock_fd == 0)
	{
		std::cerr << "! server init error !" << std::endl;
		exit (1);
	}

	std::cout << "- - - - -" << std::endl;

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
		// it's a new connection trying to be established
		// so, accept this new conection
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
					maxFd = cfd; // here number of clients get's updated

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
	FD_CLR(sock_fd, &master);
	close(sock_fd);

	// send goodbye message to clients and close sockets
	std::string goodbye = "Server is shutting down. Goodbye\r\n";
	for (long unsigned sock = 0; sock < clients.size(); sock++ )
	{
		send(sock, goodbye.c_str(), goodbye.size(), MSG_NOSIGNAL);
		FD_CLR(sock, &master);
		close(sock);
	}
	clients.clear();

	return 0;
}


