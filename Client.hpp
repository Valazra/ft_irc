#ifndef CLIENT_HPP
# define CLIENT_HPP

# define MAX_CHAR 4096
# include <iostream>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>

enum userStatus
{
	TO_REGISTER,
	PASSWORD,
	REGISTER,
	ONLINE,
	DISCONNECT_ME
};

class Client
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
		Client(int sock, struct sockaddr_in address);
		~Client();
		userStatus getStatus();
		void setStatus(UserStatus status);
		void receive();

	private:
		Client();
		Client(Client const &src);
		Client &operator=(Client const & src);

		int		_sock;
		std::string	_hostname;
		UserStatus _status;
		std::string _msg;
		vector<std::string> _cmd;
		bool _msg_finish;
	//des milliards de trucs à rajouter qu'on devra mettre dans le construct avec la struct sockaddr_in
};

#endif
