#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>

class Client
{
	class StderrException : public std::exception
	{
		virtual const char* what() const throw()
		{
			return (strerror(errno));
		}
	};

	public:
		Client(int fd, struct sockaddr_in address);
		~Client();

	private:
		Client();
		Client(Client const &src);
		Client &operator=(Client const & src);

		int		_fd;
		std::string	_hostname;
	//des milliards de trucs à rajouter qu'on devra mettre dans le construct avec la struct sockaddr_in
};

#endif
