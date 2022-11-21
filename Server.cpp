#include "Server.hpp"

Server::Server(std::string port, std::string password) : port(port), password(password), socket(100)
{
}

Server::~Server()
{

}

std::string const &Server::getPort() const
{
	return (this->port);
}

std::string const &Server::getPassword() const
{
	return (this->password);
}

int const &Server::getSocket() const
{
	return (this->socket);
}

void Server::init()
{
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::string << "ERRORRRRRRRRRRR" << std::endl;
		return ;
	}
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)))
	{
		std::string << "ERRORRRRRRRRRRR" << std::endl;
		return ;
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::string << "ERRORRRRRRRRRRR" << std::endl;
		return ;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(config.get("port").c_str()));

	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::string << "ERRORRRRRRRRRRR" << std::endl;
		return ;
	}

	if (listen(fd, address.sin_port) < 0)
	{
		std::string << "ERRORRRRRRRRRRR" << std::endl;
		return ;
	}
}
