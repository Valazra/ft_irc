#ifndef COMMAND_HPP
# define COMMAND_HPP

#define CMD_ARGS std::vector<std::string> cmd

#include "Server.hpp"
class Command
{
	public:
		Command(Client *client);
		~Command();

	private:
		Command();
		Command(Command const &src);
		Command &operator=(Command const & src);
		void whoAmI();
		void registerAttempt();

		void cap(CMD_ARGS);
		void nick(CMD_ARGS);
		void user(CMD_ARGS);
		std::map<std::string, void(Command::*)(CMD_ARGS)> _cmd_availables;

		Client *_client;

	};


#endif
