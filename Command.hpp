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
		void registerAttempt();
		void splitCommand(std::string msg);
		void sendToClient(int numeric_replies);
		void fatalError(std::string msg_error);

		void cap();
		void pass();
		void nick();
		void user();

	private:
		Command();
		Command(Command const &src);
		Command &operator=(Command const & src);

		std::map<int, Client *> *_clients_ptr;
		Client *_client;
		std::vector<std::vector<std::string> > _cmd;
		int _client_socket;
		userStatus _client_status;
		int	_actual_cmd;
		std::map<std::string, void(Command::*)()> _cmd_availables;
		std::string _password;
		std::string _server_name;
		int _fatal_error;
};

#endif
