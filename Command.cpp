#include "Command.hpp"

Command::Command(std::map<int, Client *> *client_map, std::string password):
	_clients_ptr(client_map), _password(password), _server_name("localhost"),
	_fatal_error(0)
{
	_cmd_availables["CAP"] = &Command::cap;
	_cmd_availables["PASS"] = &Command::pass;
	_cmd_availables["NICK"] = &Command::nick;
	_cmd_availables["USER"] = &Command::user;
	_cmd_availables["JOIN"] = &Command::join;
}

Command::~Command()
{
}



void	Command::registerAttempt()
{
	for (std::vector<std::vector<std::string> >::iterator it = _cmd.begin(); it != _cmd.end(); ++it)
	{
		if ((it->empty()))
			return ;
		std::cout << it->front() << std::endl;
		(this->*_cmd_availables[it->front()])();
		if (_fatal_error)
			return ;
		_actual_cmd++;
	}
	_client->setStatus(REGISTER);
}

void Command::readCmd(int client_socket)
{
	_fatal_error = 0;
	_client_socket = client_socket;
	_client = (*_clients_ptr)[client_socket];
	_cmd = _client->getCmd();
	_actual_cmd = 0;
	_client_status = _client->getStatus();
	if (_client->getStatus() == TO_REGISTER)
		registerAttempt();
	else
		std::cout << "on est deja register" << std::endl;
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
	{
		sendToClient(464); //ERR_PASSWDMISMATCH
		fatalError("You SHOULD try to connect with the good password.");
		return ;
	}

	//
	/*
	   if (client->isRegistered() == true)
	   return ft_error(ERR_ALREADYREGISTERED, client, NULL, "");
	   if (params.size() < 2)
	   return (ft_error(ERR_NEEDMOREPARAMS, client, NULL, params[0]));
	   if (client->getRegUser() == true || client->getRegNick() == true)
	   return;
	   client->setPassword(params[1]);
	   client->setRegPass(true);
	 */
}

int Command::parsingNickname(std::string nickname)
{
	std::string special("[]{}|^_\\"); //pris sur Adrien mais à vérif
	for (std::string::iterator it = nickname.begin() ; it != nickname.end() ; it++)
	{
		if (!isalpha(*it) && special.find(*it, 0) == std::string::npos && *it != '-' && !isdigit(*it))
			return (0);
	}
	return (1);
}

int Command::checkNickname(std::string nickname)
{
	for (std::map<int, Client *>::iterator it = (*_clients_ptr).begin() ; it != (*_clients_ptr).end() ; ++it)
	{
		if ((*it).second->getNickname() == nickname)
			return (0);
	}
	return (1);

}

void	Command::nick()
{
	std::cout << "On est dans nick" << std::endl;
	//a voir si on laisse size < 2 car j'ai trouvé ça sur adri mais je trouve pas un doc officiel qui explique ça
	// tu confonds adrien regarde pas si [acutal_cmd][1] est plus pettit que 2
	// il regarde si [actual_cmd] a au moins deux strings!!!! donc en gros si il y 'a bien un nickname derriere NICK
	//nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
	/*
	Nicknames are non-empty strings with the following restrictions:

    They MUST NOT contain any of the following characters: space (' ', 0x20), comma (',', 0x2C), asterisk ('*', 0x2A), question mark ('?', 0x3F), exclamation mark ('!', 0x21), at sign ('@', 0x40).
    They MUST NOT start with any of the following characters: dollar ('$', 0x24), colon (':', 0x3A).
    They MUST NOT start with a character listed as a channel type prefix.
    They SHOULD NOT contain any dot character ('.', 0x2E).
	*/
	if (_cmd[_actual_cmd][1].size() < 2)
	{
		sendToClient(431); //ERR_NONICKNAMEGIVEN
		return ;
	}
	else if (!parsingNickname(_cmd[_actual_cmd][1]))
	{
		sendToClient(432); //ERR_ERRONEUSNICKNAME
		return ;
	}
	else if (!checkNickname(_cmd[_actual_cmd][1]))
	{
		sendToClient(433); //ERR_NICKNAMEINUSE
		return ;
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
	//nn c est l inverse que t as ecrit tu vas dans le if si il est pas
	//sur TO_REGISTER
	if (_client->getStatus() != 0)
	{
		sendToClient(462); //ERR_ALREADYREGISTERED
		std::cout << "err_alreadyregistered" << std::endl;
		return ;
	}
	/*	else if (!(_cmd[_actual_cmd][1])) //trouver un meilleur moyen
		{
			sendToClient(461); //ERR_NEEDMOREPARAMS
			return ;
		}
	 */	else
	{ 
		std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
		_client->setUsername(_cmd[_actual_cmd][1]);
		std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
	}
}

void	Command::join()
{
//on parcoure tous les chans
	for (std::vector<Channel *>::iterator it = _client->getAllChannels().begin() ; it != _client->getAllChannels().end() ; ++it)
	{
		//si le chan existe déjà
		if ((*it)->getName() == _cmd[_actual_cmd][1])
		{
			//on rejoint le chan et on quitte la commande
			_client->setActualChannel(*it); 
			return ;
		}
	}
	//si le chan existe pas : on le crée
	Channel new_chan(_cmd[_actual_cmd][1], _client);
	//on ajoute le nouveau chan à la liste _all_chans de tous les clients
	for (std::map<int, Client *>::iterator it = (*_clients_ptr).begin() ; it != (*_clients_ptr).end() ; ++it)
		(*it).second->add_channel(&new_chan);
}

void Command::sendToClient(int numeric_replies)
{
	std::string msg;

	//	if (numeric_replies <= 5)
	//	{
	// :server_name c'est le prefix
	// ensuite la command qui dans notre cas est represente par son numero de reponse
	// et finalement les params
	msg == ":" + _server_name + " " + to_string(numeric_replies) + " " + _client->getUsername() + " ";
	//	}
	switch (numeric_replies)
	{
		case 1: //RPL_WELCOME
			{			
				msg += ":Welcome to the Internet Relay Network " + _client->getNickname() + "\r\n";
				break;
			}
		case 403: //ERR_NOSUCHCHANNEL
			{
				msg += _client->getActualChannel()->getName() + " :No such channel\r\n";	
				break;
			}
		case 405: //ERR_TOOMANYCHANNELS
			{
				msg += _client->getActualChannel()->getName() + " :You have  joined too many channels\r\n";
				break;
			}
		case 431: //ERR_NONICKNAMEGIVEN
			{
				msg += ":No nickname given\r\n";	
				break;
			}
		case 432: //ERR_ERRONEUSNICKNAME
			{
				msg += _client->getNickname() + " :Erroneus nickname\r\n";	
				break;
			}
		case 433: //ERR_NICKNAMEINUSE
			{
				msg += _client->getNickname() + " :Nickname is already in use\r\n";	
				break;
			}
		case 436: //ERR_NICKCOLLISION
			{
				msg += _client->getNickname() + " :Nickname collision KILL from " + _client->getUsername() + "@" + _client->getHostname() + "\r\n";	
				break;
			}
		case 461: //ERR_NEEDMOREPARAMS
			{
				msg += _cmd[_actual_cmd][0] + " :Not enough parameters\r\n";
				break;
			}
		case 462: //ERR_ALREADYREGISTERED
			{
				msg += ":You may not reregister\r\n";	
				break;
			}
		case 464: //ERR_PASSWDMISMATCH
			{
				msg += ":Password incorrect\r\n";	
				break;
			}
		case 471: //ERR_CHANNELISFULL
			{
				msg += _client->getActualChannel()->getName() + " :Cannot join channel (+1)\r\n";	
				break;
			}
		case 473: //ERR_INVITEONLYCHAN
			{
				msg += _client->getActualChannel()->getName() + " :Cannot join channel (+i)\r\n";	
				break;
			}
		case 474: //ERR_BANNEDFROMCHAN
			{
				msg += _client->getActualChannel()->getName() + " :Cannot join channel (+b)\r\n";	
				break;
			}
		case 475: //ERR_BADCHANNELKEY
			{
				msg += _client->getActualChannel()->getName() + " :Cannot join channel (+k)\r\n";	
				break;
			}
		case 476: //ERR_BADCHANMASK
			{
//sur celui la c'est chiant car on doit pas mettre le username() donc peut etre tout à changer
// suffit juste de faire un if au debut ou pr ce cas on change le debut du message
				msg += _client->getActualChannel()->getName() + " :Bad Channel Mask\r\n";	
				break;
			}
	}
	send(_client_socket, msg.c_str(), msg.size(), 0);
}


void Command::fatalError(std::string msg_error)
{
	std::string msg;
	msg = ":" + _client->getNickname() + " " + "ERROR" + " :" +msg_error + "\r\n";
	std::cout << msg << std::endl;

	send(_client_socket, msg.c_str(), msg.size(), 0);
	_fatal_error = 1;
	close(_client_socket);
	(*_clients_ptr).erase(_client_socket);
	std::cout << "fatalError" << std::endl;
}
