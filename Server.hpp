#ifndef SERVER_HPP
# define SERVER_HPP

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
# include <ctime>
# include <errno.h>
# include <vector>
# include <map>
# include "Client.hpp"
# include "Command.hpp"

# define PING 60
# define MAX_CLIENTS 60

class Server
{
	class ErrnoEx:
	public std::exception
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
		void check_new_client();
		std::vector<Client *> getClients();
		std::string get(std::string key);
		void sendPing();
		void removeClient(int const &sock_to_remove);
		void treat_complete_msg(int const &client_sock);
		void registerClient(int const &client_sock);

	private:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		std::string	_port;
		std::string	_pass;
		std::map<std::string, std::string> _values;
		std::vector<pollfd> _fds;
		std::map<int, Client *> _clients;
		time_t last_ping;
};

#endif
