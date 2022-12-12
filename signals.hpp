#ifndef SIGNALS_HPP
# define SIGNALS_hPP

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
}

#endif
