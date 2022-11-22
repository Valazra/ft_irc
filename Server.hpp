#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <string>
# include <string.h>
# include <netdb.h>
# include <stdlib.h>
# include <poll.h>
# include <vector>
# include <errno.h>
# include <map>

class Server
{
	class StderrException : public std::exception
	{
		virtual const char* what() const throw()
		{
			return (strerror(errno));
		}
	};

	public:
		Server(std::string port, std::string password);
		virtual ~Server();

		void run();
		void set(std::string key, std::string value);
		std::string get(std::string key);
		std::vector<Client *> getClients();

	private:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		std::string	_port;
		std::string	_pass;
		std::map<std::string, std::string> _values;
		std::vector<pollfd> _fds;
		std::map<int, Client *> clients;
		time_t last_ping;
};

#endif
