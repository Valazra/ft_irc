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
		signal_bool = true;
	}
}

#endif
