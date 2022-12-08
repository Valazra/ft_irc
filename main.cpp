#include "Server.hpp"
#include "signals.hpp"
#include "main.h"

bool signal_bool;

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
	signal(SIGINT, handle);
	while (1)
	{
		signal_bool = false;
		irc.run();
		if (signal_bool == true)
		{
			std::cout << "SIGNAL BOOL TRUE DANS MAIN" << std::endl;
			return (0);
		}
	}
	return (0);
}
