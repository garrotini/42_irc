#include "Server.hpp"

// cannonical form

Server::Server(): _pass(""), _port(0), _sockfd(-1), _maxFd(-1), _running(false) {}

Server::Server(const Server &other) { *this = other; } 

Server::~Server() {}

Server& Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_sockfd = other._sockfd;
		_port = other._port;
		_pass = other._pass;
	}
	return *this;
}

// getters

std::string Server::get_pass() { return _pass; } 
int			Server::get_port() { return _port; }
int			Server::get_sockfd() { return _sockfd; }

// methods

void Server::init(int port, std::string passwd)
{
	_port = port;
	_pass = passwd;
	
	std::cout << "port: " << _port << std::endl;
	std::cout << "pass: " << _pass << std::endl;
	
	// SERVER INIT
	
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

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	
	if (bind(_sockfd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
	{
		std::cerr << "! bind error !" << std::endl;
		close(_sockfd);
		exit (1);
	}

	int backlog = 10;

	if (listen(_sockfd, backlog) < 0)
	{
		std::cerr << "! listen error !" << std::endl;
		close(_sockfd);
		exit (1);
	}
}

void Server::loop()
{
	std::cout << "server is listening on :" << _port << std::endl;
	std::cout << "- - - - -" << std::endl;
	// fd_set master;
	FD_ZERO(&_master);
	
	FD_SET(_sockfd, &_master); // add the listening sock to the master set

	_maxFd = _sockfd;
	_running = true;

	// create a running server that handles multiple conns
	while(_running)
	{
		_master_dup = _master;

		int socketCount = select(_maxFd + 1, &_master_dup, NULL, NULL, NULL);
		if (socketCount < 0)
		{
			std::cerr << "! select() failed !" << std::endl;
			break ;
		}
		
		// new connection
		if (FD_ISSET(_sockfd, &_master_dup))
			new_conn();

		// check existing clients for incoming/receiving cmds or disconnect calls
		for (size_t i = 0; i < _clients.size(); i++)
		{
			int sock = _clients[i];

			// if this sockfd is in the master set, so it want to send messages
			if (FD_ISSET(sock, &_master_dup))
				recv_cmd(i);
			
			// if (!removed)
			// 	i++;
		}
	}

	// stop accepting new conns
	FD_CLR(_sockfd, &_master);
	close(_sockfd);

	// send goodbye message to clients and close sockets
	std::string goodbye = "Server is shutting down. Goodbye\r\n";
	for (long unsigned sock = 0; sock < _clients.size(); sock++ )
	{
		send(sock, goodbye.c_str(), goodbye.size(), MSG_NOSIGNAL);
		FD_CLR(sock, &_master);
		close(sock);
	}
	_clients.clear();
}

void Server::recv_cmd(int i)
{
	int sock = _clients[i];

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
		FD_CLR(sock, &_master);
		_clients.erase(_clients.begin() + i);
	}
	else
	{
		std::string input(buf, bytesIn);

		// std::cout << "input.length(): " << input.length() << std::endl;

		// check if it's quit cmd
		// TODO: quit cmd closes the entire program now, it must just say leave to server
		if (!input.empty() && input.length() == 5 && !strncmp(input.c_str(), ":q!", 3))
		{
			close(sock);
			FD_CLR(sock, &_master);
			_clients.erase(_clients.begin() + i);
			std::cout << "client " << sock << " has left the chat" << std::endl;
			std::cout << "- - - - -" << std::endl;

		}


		// otherwise, send message to everyone
		else if (!input.empty() && input.length() > 2)
		{
			std::ostringstream ss;

			ss << "SOCKET #" << sock << " says: " << input ;

			std::string output = ss.str();

			size_t len = output.size();
			char msg[len];
			strcpy(msg, output.c_str());

			// message to server
			std::cout << output;
			std::cout << "- - - - -" << std::endl;

			for (long unsigned int i = 0; i < _clients.size(); i++)
			{
				// message to clients
				if (_clients[i] != sock)
					send(_clients[i], msg, sizeof(msg), 0);
			}
		}
	}
}


void Server::new_conn()
{
	int cfd = accept(_sockfd, NULL, NULL);
	if (cfd >= 0)
	{
		// TODO - A NEW OBJECT OF CLASS CLIENTS NEEDS TO BE CREATED/INSTANTIATED HERE!
		// this client will receive his cfd, this cfd will identify him inside the backend
		// client needs _user, _nick and, _cfd
		// parameterized constructor: nick, user, cfd, signed/not-signed
		// if (nickname is signed) send 1, otherwise send 0
		// Client user(_nick,_user, cfd, signed-or-not);
		
		FD_SET(cfd, &_master);
		_clients.push_back(cfd);
		if (cfd > _maxFd)
			_maxFd = cfd; // here number of clients get's updated


		// TODO - create a `welcometochat()` message, from server to all clients 
		// (at least for debugging!)
		std::string welcome = "Welcome to Chat!\n";
		send(cfd, welcome.c_str(), welcome.size(), 0);

		std::cout << "client connected to fd: " << cfd << std::endl;
		std::cout << "number of clients connected are: " << _clients.size() << std::endl;
		std::cout << "- - - - -" << std::endl;
	}
}
