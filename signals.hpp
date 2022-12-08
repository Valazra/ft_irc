#ifndef SIGNALS_HPP
# define SIGNALS_hPP

# include <signal.h>
# include "Server.hpp"
# include "main.h"

void handle(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << "ON EST DANS SIGNAL ----> SIGINT" << std::endl;
		if (g_clients != NULL)
		{
			for(std::map<int, Client *>::iterator it = g_clients->begin() ; it != g_clients->end() ; ++it)
			{
				delete((*it).second);
				(*it).second = NULL;
			}
		}
		if (g_channels != NULL)
		{
			for(std::vector<Channel *>::iterator it = g_channels->begin() ; it != g_channels->end() ; ++it)
			{
				delete(*it);
				(*it) = NULL;
			}
		}
	}
	exit(0);
}

#endif
