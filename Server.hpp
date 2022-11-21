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

class Server
{
	class ExErrno:
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

	//	std::string const &getPort() const;
	//	std::string const &getPassword() const;

	private:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		std::string	_port;
		std::string	_pass;
		std::vector<pollfd> fds;
};

#endif
