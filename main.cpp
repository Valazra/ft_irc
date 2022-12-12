#include "Server.hpp"
#include "signals.hpp"
#include "main.h"

bool quit = false;

int main(int ac, char **av)
{
	std::string port;
	std::string password;

	if (ac != 3)
	{
		std::cout << "Error, bad number of args." << std::endl;
		return (-1);
	}
	else
	{
		port = av[1];
		password = av[2];
	}
	signal(SIGINT, handle);
	Server irc(port, password);
	while (!quit)
		irc.run();
	if (errno)
		return (errno);
	return (0);
}
