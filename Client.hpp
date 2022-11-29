#ifndef CLIENT_HPP
# define CLIENT_HPP

# define MAX_CHAR 4096
# include "Channel.hpp"
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

class Channel;
class Command;

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
		std::vector<std::vector<std::string> > * getCmd();
		
		void splitCommand();
		int getSock();
		bool getMsgFinish();
		std::string getHostname();
		std::string getNickname();
		std::string getUsername();
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void add_channel(Channel *channel);
		void leave_channel(Channel *channel);
		Channel *getActualChannel();
		void setActualChannel(Channel *channel);
		std::vector<Channel *> getAllChannels();
		bool getOper();

	private:
		Client();
		Client(Client const &src);
		Client &operator=(Client const & src);

		int		_sock;
		bool		_msg_finish;
		userStatus	_status;
		std::string	_hostname;
		std::string	_nickname;
		std::string	_username;
		std::string	_msg;
		std::vector<std::vector<std::string> > _cmd;
//mettre en vector si un client peut avoir plusieurs chans en meme temps
		Channel *_actual_channel;
		std::vector<Channel *> _all_channels;
		bool		_oper;
};

#endif
