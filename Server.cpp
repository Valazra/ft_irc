#include "Server.hpp"

Server::Server(std::string port, std::string password) : _port(port), _password(password)
{

}

Server::~Server()
{

}

std::string const &Server::getPort() const
{
	return (this->_port);
}

std::string const &Server::getPassword() const
{
	return (this->_password);
}
