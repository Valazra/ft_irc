#include "Command.hpp"

Command::Command(std::map<int, Client *> *client_map):
_clients_ptr(client_map)
{
	_cmd_availables["CAP"] = &Command::cap;
	_cmd_availables["NICK"] = &Command::nick;
	_cmd_availables["USER"] = &Command::user;
}

Command::~Command()
{
}



void	Command::registerAttempt()
{
//	if (_cmd_availables[_cmd[_actual_cmd][0]])


	std::cout << _cmd[_actual_cmd][0] << std::endl;
	(this->*_cmd_availables[_cmd[_actual_cmd][0]])();

	/*
	if (key.compare("CAP") != 0 && key.compare("PASS") != 0 && key.compare("USER") != 0 && key.compare("NICK") != 0)
		{
			this->_parsed_cmd.clear();
			return (ft_registration_failed(client));
		}
		if ((key.compare("USER") == 0 && client->getRegUser() == true) || (key.compare("NICK") == 0 && client->getRegNick() == true))
		{	
			this->_parsed_cmd.clear();
			return ft_registration_failed(client);
		}
	*/
}

void Command::readCmd(int client_socket)
{
	_client_socket = client_socket;
	_client = (*_clients_ptr)[client_socket];
	_cmd = _client->getCmd();
	_actual_cmd = 0;
	_client_status = _client->getStatus();
		std::cout << _client_status << std::endl;
	if (_client->getStatus() == TO_REGISTER)
	{
		std::cout << "ola ola ola" << std::endl;
		registerAttempt();
	}
}

void	Command::cap()
{
	std::cout << "ON NE FAIT RIEN POUR CAP, ON IGNORE ET ON CONTINUE" << std::endl;
}

int parsingNickname(std::string nickname)
{
	std::string special("[]{}|^_\\"); //pris sur Adrien mais à vérif
	for (std::string::iterator it = nickname.begin() ; it != nickname.end() ; it++)
	{
		if (!isalpha(*it) && special.find(*it, 0) == std::string::npos && *it != '-' && !isdigit(*it))
			return (0);
	}
	return (1);
}

int checkNickname(std::string nickname)
{
	(void)nickname;
	//probleme car on doit parcourir TOUS les clients pour voir si le nom est pas déjà pris
	//donc faut faire iterer sur tous les clients etc mais ici on y a pas acces
/*	std::vector<Client *>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if ((*it)->getNickname() == nickname)
			return (0);
	}
*/	return (1);

}

void	Command::nick()
{
/*	if (!cmd[1])
		ERR_NONICKNAMEGIVEN(); //return 431
	else if (!parsingNickname(cmd[1]))
		ERR_ERRONEUSNICKNAME(); //return 432
	else if (!checkNickname(cmd[1]))
		ERR_NICKNAMEINUSE(); //return 433
	else
	{
		_client->setNickname(cmd[1]);
		//avertir le client que la commande nick est successfull et avertir les autres clients du changement de nickname
	}
*/
}
void	Command::user()
{
}
