#include "Server.hpp"

int main(int ac, char **av)
{
	std::string port;
	std::string password;

	if (ac != 3)
	{
		std::cout << "Error, bad number of args" << std::endl;
	}
	else
	{
		port = av[1];
		password = av[2];
	}
	Server irc(port, password);
	irc.init();

	return (0);
}
