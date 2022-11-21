#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <fcntl.h>
# include <unistd.h>
# include <string>

class Server
{
	public:
		Server(std::string port, std::string password);
		virtual ~Server();

		std::string const &getPort() const;
		std::string const &getPassword() const;
		int const &getSocket() const;
		void init();

	private:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		std::string	_port;
		std::string	_password;
		int		_socket;
};

#endif
