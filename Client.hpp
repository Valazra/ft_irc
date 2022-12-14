#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <vector>
# include <string>
# define MAX_CHAR 4096

enum userStatus
{
	TO_REGISTER,
	REMOVE_ME,
	REGISTER,
};

class Channel;
class Command;

class Client
{
	public:
		Client(int sock);
		~Client();
		userStatus getStatus();
		void setStatus(userStatus status);
		void receive();
		std::vector<std::vector<std::string> > * getCmd();

		void setAlreadyUserCmd(bool already);
		bool getAlreadyUserCmd();
		void setAlreadyNickCmd(bool already);
		bool getAlreadyNickCmd();
		
		void splitCommand();
		int getSock();
		bool getMsgFinish();
		std::string getHostname();
		std::string getNickname();
		std::string getUsername();
		std::string getRealname();
		std::string getOptions();
		bool IsOptionSet(std::string check, std::string option);
		void changeOptions(std::string options, bool add);
		void setNickname(std::string nickname);
		void setUsername(std::string username);
		void setRealname(std::string realname);
		bool getOper();
		void setOper(bool oper);
		void addChannel(Channel *channel);
		void leaveChannel(Channel *channel);
		std::vector<Channel *>* getClientChannels();
		void leaveAllChannels();
		void setServername(std::string name);
		std::string getServername();
		void setCorrectPassOn();
		bool getCorrectPass();


	private:
		Client();
		Client(Client const &src);
		Client &operator=(Client const & src);


		bool _already_user_cmd;
		bool _already_nick_cmd;
		std::string _options;
		int		_sock;
		bool		_msg_finish;
		userStatus	_status;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_server_name;
		std::string	_msg;
		std::vector<std::vector<std::string> > _cmd;
		bool		_oper;
		std::vector<Channel *> _client_channels;
		bool		_correct_pass;
};

#endif
