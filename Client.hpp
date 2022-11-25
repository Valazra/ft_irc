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
# include <vector>

enum userStatus
{
	TO_REGISTER,
	REMOVE_ME,
	PASSWORD,
	REGISTER,
	ONLINE
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
		void setStatus(userStatus status);
		void receive();
		std::vector<std::vector<std::string> > getCmd();
		
		void splitCommand();
		bool getMsgFinish();
		std::string getNickname();
		std::string getUsername();
		void setNickname(std::string nickname);
		void setUsername(std::string username);

	private:
		Client();
		Client(Client const &src);
		Client &operator=(Client const & src);

		int		_sock;
		std::string	_hostname;
		bool		_msg_finish;
		userStatus	_status;
		std::string	_nickname;
		std::string	_username;
		std::string	_msg;
		std::vector<std::vector<std::string> > _cmd;
	//des milliards de trucs à rajouter qu'on devra mettre dans le construct avec la struct sockaddr_in
};

#endif
