#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "Client.hpp"
#include "to_string.hpp"
#include <map>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>

class Command
{
	public:
		Command(std::map<int, Client *> *client_map, std::string password);
		~Command();
		void readCmd(int client_socket);
		void execCmd();
		void splitCommand(std::string msg);
		void sendToClient(int numeric_replies);
		void sendToTarget();
		void sendToChannel();
		void fatalError(std::string msg_error);
		int parsingNickname(std::string nickname);
		int checkNickname(std::string nickname);

	private:
		Command();
		Command(Command const &src);
		Command &operator=(Command const & src);

		void oper();
		void cap();
		void pass();
		void nick();
		void user();
		void join();
		void privmsg();
		void quit();
		std::string insert_zeros(int nbr);

		std::map<int, Client *> *_clients_ptr;
		Client *_client;
		std::vector<std::vector<std::string> >* _cmd;
		int _client_socket;
		userStatus _client_status;
		int	_actual_cmd;
		std::map<std::string, void(Command::*)()> _cmd_availables;
		std::string _password;
		std::string _server_name;
		bool _fatal_error;
		bool _correctPass;
};

#endif
