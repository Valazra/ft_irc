#include "Server.hpp"
#include "signals.hpp"
#include "main.h"

std::map<int, Client *> *g_clients = NULL;
std::vector<Channel *> *g_channels = NULL;

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
		irc.run();
	}
	return (0);
}
