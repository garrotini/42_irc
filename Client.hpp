#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client
{
	private:
		std::string _nick;
		std::string _user;
		int			_fd;
		bool 		_signed;
	private:
		Client();
		Client(const Client& other);
		Client& operator=(const Client& other);
		~Client();

		std::string getNick(Client& o);
		std::string getUser(Client& o);
		int getFd(Client& o);
		bool getSigned(Client& o);

		void setNick(std::string nick);
		void setUser(std::string user);
};


#endif
