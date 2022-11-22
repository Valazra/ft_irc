#ifndef SERVER_HPP
# define SERVER_HPP

# include <poll.h>
# include <vector>
# include <errno.h>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <unistd.h>
# include <string>
# include <string.h>
# include <netdb.h>
# include <stdlib.h>
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

		void set(std::string key, std::string value);
		std::string get(std::string key);

	private:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		std::string	_port;
		std::string	_pass;
		std::map<std::string, std::string> values;
		std::vector<pollfd> fds;
};

#endif
