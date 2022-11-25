#include "Command.hpp"

Command::Command(std::map<int, Client *> *client_map, std::string password):
	_clients_ptr(client_map), _password(password)
{
	_cmd_availables["CAP"] = &Command::cap;
	_cmd_availables["PASS"] = &Command::pass;
	_cmd_availables["NICK"] = &Command::nick;
	_cmd_availables["USER"] = &Command::user;
}

Command::~Command()
{
}



void	Command::registerAttempt()
{
//il nous faut le  nombre de commandes pour parcourir exactement le bon nombre
	while (_actual_cmd < 4)
	{
		std::cout << _cmd[_actual_cmd][0] << std::endl;
		(this->*_cmd_availables[_cmd[_actual_cmd][0]])();
		_actual_cmd++;
	}
}

void Command::readCmd(int client_socket)
{
	_client_socket = client_socket;
	_client = (*_clients_ptr)[client_socket];
	_cmd = _client->getCmd();
	_actual_cmd = 0;
	_client_status = _client->getStatus();
	if (_client->getStatus() == TO_REGISTER)
		registerAttempt();
}

void	Command::cap()
{
	std::cout << "ON NE FAIT RIEN POUR CAP, ON IGNORE ET ON CONTINUE" << std::endl;
}

void	Command::pass()
{
	std::cout << "On est dans pass" << std::endl;
	std::cout << "bon pass du serv = " << _password << std::endl;
	std::cout << "pass envoyé par le client = " << _cmd[_actual_cmd][1] << std::endl;
	if (_cmd[_actual_cmd][1] == _password)
		std::cout << "bon password bravo" << std::endl;
	else
		std::cout << "mauvais pass" << std::endl;
		//mettre un truc qui stoppe tout
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
	std::cout << "on est dans nick" << std::endl;
//a voir si on laisse size < 2 car j'ai trouvé ça sur adri mais je trouve pas un doc officiel qui explique ça
	if (_cmd[_actual_cmd][1].size() < 2)
	{
		std::cout << "err_nonicknamegiven" << std::endl;
		return ;//	ERR_NONICKNAMEGIVEN(); //return 431
	}
	else if (!parsingNickname(_cmd[_actual_cmd][1]))
	{
		std::cout << "err_erroneusnickname" << std::endl;
		return ;//ERR_ERRONEUSNICKNAME(); //return 432
	}
	else if (!checkNickname(_cmd[_actual_cmd][1]))
	{
		std::cout << "err_nicknameinuse" << std::endl;
		return ;//ERR_NICKNAMEINUSE(); //return 433
	}
	else
	{
		std::cout << "_client->getNickname() = " << _client->getNickname() << std::endl;
		_client->setNickname(_cmd[_actual_cmd][1]);
		std::cout << "_client->getNickname() = " << _client->getNickname() << std::endl;
		//avertir le client que la commande nick est successfull et avertir les autres clients du changement de nickname
	}

}
void	Command::user()
{
	std::cout << "slt on est dans user" << std::endl;
	//si le status est encore TO_REGISTER alors on va dans le if
	if (_client->getStatus() != 0)
	{
		std::cout << "err_alreadyregistered" << std::endl;
		return ; // ERR_ALREADYREGISTERED(); //return 462
	}
/*	else if (!(_cmd[_actual_cmd][1])) //trouver un meilleur moyen
	{
		std::cout << "err_needmoreparams" << std::endl;
		return ; //ERR_NEEDMOREPARAMS; // RETURN 461
	}
*/	else
	{ 
		std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
		_client->setUsername(_cmd[_actual_cmd][1]);
		std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
	}
}
