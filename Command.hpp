#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "to_string.hpp"
#include <map>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>
# define DEBUG 1

class Server;
class Command
{
	public:
		Command(std::map<int, Client *> *client_map, std::string password);
		~Command();
		void readCmd(int client_socket);
		void execCmd();
		void splitCommand(std::string msg);


	private:
		Command();
		Command(Command const &src);
		Command &operator=(Command const & src);

		//COMMANDS
		void oper();
		void cap();
		void pass();
		void nick();
		void user();
		void join();
		void privmsg();
		void quit();
		void mode();
		void fatalError(std::string msg_error); //ERROR

		//COMMANDS UTILS
		void sendToClient(int numeric_replies);
		void sendToTarget(std::string target_name, int target_socket);
		int parsingNickname(std::string nickname);
		int checkNickname(std::string nickname);
		bool check_if_valid_cmd(std::string cmd);
		std::string insert_zeros(int nbr);

		//Channels helper
		void sendToChannel();
		void add_channel(Channel *channel);
		void leave_channel(Channel *channel);
		Channel *getActualChannel();
		void setActualChannel(Channel *channel);
		std::vector<Channel *> getAllChannels();

		//Clients
		Client *_client;
		int _client_socket;
		userStatus _client_status;
		std::map<int, Client *> *_clients_ptr;
		//Cmd
		std::map<std::string, void(Command::*)()> _cmd_availables;
		std::vector<std::vector<std::string> >* _cmd;
		int	_actual_cmd;
		std::vector<std::string> _cmd_list;
		//Conf
		std::string _password;
		bool _correctPass;
		std::string _server_name;
		std::string	_oper_name;
		std::string	_oper_pass;
		//Channels
		Channel *_actual_channel;
		std::vector<Channel *> _all_channels;
		//ERROR
		bool _fatal_error;
};

#endif
