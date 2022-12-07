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
#include <iostream>
#include <stdio.h>
#include <time.h>
# define DEBUG 1

class Server;
class Command
{
	public:
		Command(std::map<int, Client *> *client_map, std::string password, bool *fatal_error);
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
		void notice();
		void kick();
		void kill();
		void topic();
		void invite();
		void part();
		void names();
		void fatalError(std::string msg_error); //ERROR

		//COMMANDS UTILS
		void partSingle(std::string current_chan);
		void sendToClient(int numeric_replies);
		void sendToTarget(std::string target_name, int target_socket, bool is_notice);
		int parsingNickname(std::string nickname);
		int parsingRealname(std::string realname);
		int checkNickname(std::string nickname);
		bool check_if_valid_cmd(std::string cmd);
		std::string insert_zeros(int nbr);
		std::string getTime();
		int nbCommas(std::string chans);
		std::vector<std::string> splitCommas(std::string listchans);
		void joinSingle(std::string current_chan);

		//Channels helper
		void sendToChannel(Channel *channel, bool is_notice);
		std::vector<Channel *> getAllChannels();
		Channel *findChan(std::string chan_name);
		void checkIfEmptyChan();
		std::vector<Channel *>::iterator returnItChan(std::string chan_name);
		void clearChan(Channel *chan, Client *client);

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
		std::vector<Channel *> _all_channels;
		Channel *_actual_chan;
		std::string _bad_chan_name;
		bool _bad_chan_bool;
		std::string _bad_nickname;
		//ERROR
		bool *_fatal_error;
		//Time
		std::string _creationTime;
		//Close
		void closeConnection(int close_socket);
		//Name reply
		void nameReply(std::string chan_name, Channel *chan);
		//Join rply
		void	msgJoin(std::string chan_name, Channel *finded_chan);
};

#endif
