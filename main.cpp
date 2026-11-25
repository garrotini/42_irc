#include "Server.hpp"

// class Client
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
	
	Server server;
	
	server.init(port, passwd);

	server.loop();

	return 0;
}


