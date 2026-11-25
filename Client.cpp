#include "Client.hpp"

Client::Client() : _nick(""), _user(""), _fd(-1), _signed(false) {}

Client::Client(const Client& other) { *this = other; }

Client& Client::operator=(const Client& other)
{
	if (this != &other)
	{
		_nick = other._nick;
		_user = other._user;
		_fd = other._fd;
		_signed = other._signed;
	}
	return *this;
}

Client::~Client() {}

std::string Client::getNick(Client& o) {return o._nick; }
std::string Client::getUser(Client& o) {return o._user; }
int Client::getFd(Client& o) {return o._fd; }
bool Client::getSigned(Client& o) {return o._signed; }

void Client::setNick(std::string nick) { _nick = nick; }
void Client::setUser(std::string user) { _user = user; }

