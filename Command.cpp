#include "Command.hpp"

Command::Command(std::map<int, Client *> *client_map, std::string password):
	_clients_ptr(client_map), _password(password), _server_name("localhost"), _fatal_error(0), _correctPass(false)
{
	_cmd_availables["OPER"] = &Command::oper;
	_cmd_availables["CAP"] = &Command::cap;
	_cmd_availables["PASS"] = &Command::pass;
	_cmd_availables["NICK"] = &Command::nick;
	_cmd_availables["USER"] = &Command::user;
	_cmd_availables["JOIN"] = &Command::join;
	_cmd_availables["QUIT"] = &Command::quit;
}

Command::~Command()
{
}

void Command::execCmd()
{
	for (std::vector<std::vector<std::string> >::iterator it = (*_cmd).begin(); it != (*_cmd).end(); ++it)
	{
		if ((it->empty()))
			return ;
		std::cout << it->front() << std::endl;
		(this->*_cmd_availables[it->front()])();
		if (_fatal_error)
			return ;
		_actual_cmd++;
	}
}

void Command::readCmd(int client_socket)
{
	_fatal_error = false;
	_correctPass = false;
	_client_socket = client_socket;
	_client = (*_clients_ptr)[client_socket];
	_cmd = _client->getCmd();
	_actual_cmd = 0;
	_client_status = _client->getStatus();
	if (_client->getStatus() == TO_REGISTER)
	{
		std::cout << "Command::readCmd TO_REGISTER" << std::endl;
		execCmd();
		(*_cmd).clear();
		_client->setStatus(REGISTER);
	}
	else
	{
		/*
		std::cout << "Command::readCmd REGISTER" << std::endl;
		execCmd();
		*/
		if ((*_cmd).size() > 0)
			(*_cmd).clear();
	}
}

void	Command::oper()
{
//	if (_client->getOper() == false)
//	{
		if ((*_cmd)[_actual_cmd].size() != 3)
		{
			sendToClient(461); //ERR_NEEDMOREPARAMS
			return ;
		}
		else if (_client->getStatus() != REGISTER)
		{
			sendToClient(491); //ERR_NOOPERHOST
			return ;
		}
//	}

}

void	Command::cap()
{
	std::cout << "Command::cap" << std::endl;
}

void	Command::pass()
{
	std::cout << "Command::pass | Pass attendu:" << _password << " | Pass recu = " << (*_cmd)[_actual_cmd][1] << std::endl;
	if (_client->getStatus() != 0)
	{
		sendToClient(462); //ERR_ALREADYREGISTERED
		return ;
	}
	else if ((*_cmd)[_actual_cmd].size() < 2)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	if ((*_cmd)[_actual_cmd][1] == _password)
	{
		_correctPass = true;
	}
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
//find renvoie npos si aucune occurence n'a été trouvée, sinon ça renvoie l'endroit ou l'occurence à été trouvée
	std::string forbidden(" ,*?!@.");
	if (nickname[0] == '$' || nickname[0] == ':' || forbidden.find(nickname[0], 0) != std::string::npos)
		return (0);
	for (std::string::iterator it = nickname.begin() + 1 ; it != nickname.end() ; it++)
	{
		if (forbidden.find((*it), 0) != std::string::npos)
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
	std::cout << "Command::nick" << std::endl;
	/*
	Nicknames are non-empty strings with the following restrictions:

    They MUST NOT contain any of the following characters: space (' ', 0x20), comma (',', 0x2C), asterisk ('*', 0x2A), question mark ('?', 0x3F), exclamation mark ('!', 0x21), at sign ('@', 0x40).
    They MUST NOT start with any of the following characters: dollar ('$', 0x24), colon (':', 0x3A).
    They MUST NOT start with a character listed as a channel type prefix.
    They SHOULD NOT contain any dot character ('.', 0x2E).
	*/
	if (!parsingNickname((*_cmd)[_actual_cmd][1]))
	{
		sendToClient(432); //ERR_ERRONEUSNICKNAME
		return ;
	}
	else if (!checkNickname((*_cmd)[_actual_cmd][1]))
	{
		sendToClient(433); //ERR_NICKNAMEINUSE
		return ;
	}
	else if ((*_cmd)[_actual_cmd].size() != 2)
	{
		sendToClient(431); //ERR_NONICKNAMEGIVEN
		return ;
	}
	else
	{
		//probleme a faire getnickname alors que il y en a pas au debut?
		std::cout << "old nick name = " << _client->getNickname() << std::endl;
		_client->setNickname((*_cmd)[_actual_cmd][1]);
		std::cout << "new nick name = " << _client->getNickname() << std::endl;
		//avertir le client que la commande nick est successfull et avertir les autres clients du changement de nickname
	}

}
void	Command::user()
{
	std::cout << "Command::user" << std::endl;
	if (!_correctPass)
	{
		sendToClient(464); //ERR_PASSWDMISMATCH
		fatalError("You should connect with a password.");
		return ;
	}
	//si le status est pas sur TO_REGISTER alors on va dans le if
	if (_client->getStatus() != 0)
	{
		sendToClient(462); //ERR_ALREADYREGISTERED
		return ;
	}
	else if ((*_cmd)[_actual_cmd].size() < 2)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	else
	{ 
		std::cout << "_client->getUsername() = " << _client->getUsername() << "|" << std::endl;
		_client->setUsername((*_cmd)[_actual_cmd][1]);
		std::cout << "_client->getUsername() = " << _client->getUsername() << "|"<< std::endl;
	}
	sendToClient(1);
	sendToClient(2);
	sendToClient(3);
	sendToClient(4);
}

void	Command::join()
{
//on parcoure tous les chans
	for (std::vector<Channel *>::iterator it = _client->getAllChannels().begin() ; it != _client->getAllChannels().end() ; ++it)
	{
		//si le chan existe déjà
		if ((*it)->getName() == (*_cmd)[_actual_cmd][1])
		{
			//on rejoint le chan et on quitte la commande
			_client->setActualChannel(*it); 
			return ;
		}
	}
	//si le chan existe pas : on le crée
	Channel new_chan((*_cmd)[_actual_cmd][0], _client);
	//on ajoute le nouveau chan à la liste _all_chans de tous les clients
	for (std::map<int, Client *>::iterator it = (*_clients_ptr).begin() ; it != (*_clients_ptr).end() ; ++it)
		(*it).second->add_channel(&new_chan);
}

void	Command::quit()
{
//faudra bien tout libérer ici
//fatal_error
	_client->setStatus(REMOVE_ME);
}

void Command::sendToClient(int numeric_replies)
{
	std::string msg;

	if (numeric_replies)
		msg = ":" + _server_name + " " + insert_zeros(numeric_replies) + to_string(numeric_replies) + " " + _client->getNickname() + " :";
//	else if ()
//		msg = "si besoin";
	switch (numeric_replies)
	{
		case 1: //RPL_WELCOME
		{			
				msg += "Welcome to the Internet Relay Network " + _client->getNickname() + "\r\n";
				std::cout << msg << std::endl;
				break;
		}
		case 2:
		{
			msg += "Your host is " + _server_name + ", running on version [42.42]\r\n";
				std::cout << msg << std::endl;
			break;
		}
		case 3:
		{
			msg += "This server was created 15h30 fdp\r\n";
				std::cout << msg << std::endl;
			break;
		}
		case 4:
		{
			msg += _server_name + " version [42.42]. Available user MODE : +Oa . Avalaible channel MODE : none. \r\n";
				std::cout << msg << std::endl;
			break;
		}
		case 5:
		{
			msg += "Sorry IRC_90's capacity is full. Please retry connection later\r\n";
			std::cout << msg << std::endl;
			break;
		}	
		case 403: //ERR_NOSUCHCHANNEL
		{
				msg += _client->getUsername() + " " + _client->getActualChannel()->getName() + " :No such channel\r\n";	
				break;
		}
		case 405: //ERR_TOOMANYCHANNELS
		{
			msg += _client->getUsername() + " " + _client->getActualChannel()->getName() + " :You have  joined too many channels\r\n";
				break;
		}
		case 431: //ERR_NONICKNAMEGIVEN
		{
			msg += " :No nickname given\r\n";	
			break;
		}
		case 432: //ERR_ERRONEUSNICKNAME
		{
			msg += _client->getUsername() + " " + _client->getNickname() + " :Erroneus nickname\r\n";	
			break;
		}
		case 433: //ERR_NICKNAMEINUSE
		{
			msg += _client->getUsername() + " " +  _client->getNickname() + " :Nickname is already in use\r\n";	
			break;
		}
		case 436: //ERR_NICKCOLLISION
		{
			msg += _client->getNickname() + " :Nickname collision KILL from " + _client->getUsername() + "@" + _client->getHostname() + "\r\n";	
			break;
		}
		case 461: //ERR_NEEDMOREPARAMS
		{
			msg += _client->getUsername() + " " + (*_cmd)[_actual_cmd][0] + " :Not enough parameters\r\n";
			std::cout << msg << std::endl;
			break;
		}
		case 462: //ERR_ALREADYREGISTERED
		{
			msg += _client->getUsername() + " :You may not reregister\r\n";	
			break;
		}
		case 464: //ERR_PASSWDMISMATCH
		{
			msg += _client->getUsername() + " :Password incorrect\r\n";	
			break;
		}
		case 471: //ERR_CHANNELISFULL
		{
			msg += _client->getUsername() + " " + _client->getActualChannel()->getName() + " :Cannot join channel (+1)\r\n";	
			break;
		}
		case 473: //ERR_INVITEONLYCHAN
		{
			msg += _client->getUsername() + " " + _client->getActualChannel()->getName() + " :Cannot join channel (+i)\r\n";	
			break;
		}
		case 474: //ERR_BANNEDFROMCHAN
		{
			msg += _client->getUsername() + " " + _client->getActualChannel()->getName() + " :Cannot join channel (+b)\r\n";	
			break;
		}
		case 475: //ERR_BADCHANNELKEY
		{
			msg += _client->getUsername() + " " + _client->getActualChannel()->getName() + " :Cannot join channel (+k)\r\n";	
			break;
		}
		case 476: //ERR_BADCHANMASK
		{
			msg += _client->getActualChannel()->getName() + " :Bad Channel Mask\r\n";	
			break;
		}
		case 491: //ERR_NOOPERHOST
		{
			msg += _client->getUsername() + " :No O-lines for your host\r\n";	
			break;
		}
		default :
		{
			 msg = "gros fdp\r\n";
			 break ;
		}
		
	}
	send(_client_socket, msg.c_str(), msg.size(), 0);
}

std::string Command::insert_zeros(int nbr)
{
	std::string tmp("");
	if (nbr >= 0 && nbr <= 9)
	{
		tmp = to_string("00");
		return (tmp);
	}
	if (nbr >= 10 && nbr <= 99)
	{
		tmp = to_string("0");
		return (tmp);
	}
	return(tmp);
}


//send to target (message privé)

void Command::fatalError(std::string msg_error)
{
	std::string msg;
	msg = ":" + _client->getNickname() + " " + "ERROR" + " :" +msg_error + "\r\n";
	std::cout << msg << std::endl;

	send(_client_socket, msg.c_str(), msg.size(), 0);
	_fatal_error = true;
	close(_client_socket);
	(*_clients_ptr).erase(_client_socket);
	std::cout << "fatalError" << std::endl;
}
