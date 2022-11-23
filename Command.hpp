#ifndef COMMAND_HPP
# define COMMAND_HPP

#define CMD_ARGS std::vector<std::string> cmd
class Command
{
	public:
		Command();
		~Command();

	private:
		Command();
		Command(Command const &src);
		Command &operator=(Command const & src);
		whoAmI(CMD_ARGS);

		void cap(CMD_ARGS);
		void nick(CMD_ARGS);
		void user(CMD_ARGS);
		std::map<std::string, void(Commands::*)(CMD_ARGS)> _cmd_availables

	};


#endif
