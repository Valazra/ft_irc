#ifndef SIGNALS_H
# define SIGNALS_H

# include <signal.h>
# include "Server.hpp"
# include "main.h"

void handle(int sig)
{
	if (sig == SIGINT)
	{
		if (DEBUG)
			std::cout << "SIGINT" << std::endl;
		quit = true;
	}
	if (sig == SIGQUIT)
	{
		if (DEBUG)
			std::cout << "SIGQUIT" << std::endl;
		quit = true;
	}
}

#endif
