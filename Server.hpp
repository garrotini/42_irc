#ifndef SERVER_HPP
#define SERVER_HPP

#include <asm-generic/socket.h>
#include <cstdio>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>

#define BUFF_SIZE 4096

class Server
{
	private:
		std::string 		_pass;
		int 				_port;
		int 				_sockfd;
		std::vector<int> 	_clients;
		int 				_maxFd;
		bool 				_running;
		fd_set 				_master;
		fd_set 				_master_dup;

	public:
		Server();
		Server(const Server &other);
		Server& operator=(const Server &other);
		~Server();

		void init(int port, std::string passwd);
		void loop();

		void new_conn();
		bool recv_cmd(int sock, bool removed);

		std::string get_pass();
		int get_sockfd();
		int get_port();
};


#endif

