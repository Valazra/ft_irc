#include "Command.hpp"

Command::Command(std::map<int, Client *> *client_map, std::string password):
	_clients_ptr(client_map), _password(password), _correctPass(false), _server_name("localhost"), _oper_name("coco"), _oper_pass("toto"),_fatal_error(0), _creationTime(getTime()) 
{
	_cmd_list.push_back("MODE");
	_cmd_list.push_back("OPER");
	_cmd_list.push_back("CAP");
	_cmd_list.push_back("PASS");
	_cmd_list.push_back("NICK");
	_cmd_list.push_back("USER");
	_cmd_list.push_back("JOIN");
	_cmd_list.push_back("PRIVMSG");
	_cmd_list.push_back("QUIT");
	_cmd_list.push_back("NOTICE");
	_cmd_list.push_back("KILL");
	_cmd_list.push_back("KICK");
	_cmd_availables["MODE"] = &Command::mode;
	_cmd_availables["OPER"] = &Command::oper;
	_cmd_availables["CAP"] = &Command::cap;
	_cmd_availables["PASS"] = &Command::pass;
	_cmd_availables["NICK"] = &Command::nick;
	_cmd_availables["USER"] = &Command::user;
	_cmd_availables["JOIN"] = &Command::join;
	_cmd_availables["PRIVMSG"] = &Command::privmsg;
	_cmd_availables["QUIT"] = &Command::quit;
	_cmd_availables["NOTICE"] = &Command::notice;
	_cmd_availables["KILL"] = &Command::kill;
	_cmd_availables["KICK"] = &Command::kick;
}

Command::~Command()
{
}

//COMMANDS MAIN FUNCTIONS
bool Command::check_if_valid_cmd(std::string cmd)
{
	for (std::vector<std::string>::iterator it1 = _cmd_list.begin(); it1 != _cmd_list.end(); ++it1)
	{
		if (*it1 == cmd)
			return (true);
	}
	return (false);
}

void Command::execCmd()
{
	for (std::vector<std::vector<std::string> >::iterator it = (*_cmd).begin(); it != (*_cmd).end(); ++it)
	{
		if ((it->empty()))
			return ;
		std::cout << "execCmd() de la commande : " << it->front() << std::endl;
		// la commande est pas une qu on gere voir ce qu on fait au lieu de return ;
		if (!check_if_valid_cmd(it->front()))
			return ;
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
		if (DEBUG)
			std::cout << "Command::readCmd TO_REGISTER" << std::endl;
		execCmd();
		(*_cmd).clear();
		_client->setStatus(REGISTER);
	}
	else
	{
		if (DEBUG)
			std::cout << "Command::readCmd REGISTER" << std::endl;
		execCmd();
		if ((*_cmd).size() > 0)
			(*_cmd).clear();
	}
}

// MODE
void	Command::mode()
{
	if (DEBUG)
		std::cout << "Command::mode" << std::endl;
	if ((*_cmd)[_actual_cmd].size() < 2)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	std::string target_name = (*_cmd)[_actual_cmd][1];
	if ((*_cmd)[_actual_cmd][1][0] == '#')
	{
		//Channel part
		for(std::vector<Channel *>::iterator it = _all_channels.begin() ; it != _all_channels.end() ; ++it)
		{
			if ((*it)->getName() == target_name)
			{
				_actual_chan = (*it);
				if ((*_cmd)[_actual_cmd].size() == 2)
				{
					sendToClient(324); //RPL_CHANNELMODEIS
									   // On peut aussi envoyer la 329 pour donner la date de creation du chan mais osef, nn?
					return ;
				}
				//user must have appropriate chan privilege to changes the mode
				/*
				if (rightsForThisChanAndThisMode(_client, mode))
				{
				
				}
				else
				{
					sendToClient(482); //ERR_CHANOPRIVSNEEDED
				}
				*/

			}
		}
		sendToClient(403); //ERR_NOSUCHCHANNEL

	}
	else
	{
		//User part
		if (!checkNickname((*_cmd)[_actual_cmd][1]))
		{
			sendToClient(401); //ERR_NOSUCHNICK 
			return ;
		}
		if ((*_cmd)[_actual_cmd][1] != (*_client).getNickname())
		{
			sendToClient(502); //ERR_USERSDONTMATCH 
			return ;
		}
		if ((*_cmd)[_actual_cmd].size() == 2)
		{
			sendToClient(221); //RPL_UMODEIS 
			return ;
		}
		if ((*_cmd)[_actual_cmd][3] == "-")
		{
			if ((*_cmd)[_actual_cmd].size() > 4 && (*_cmd)[_actual_cmd][4] == "o")
			{
				(*_client).setOper(false);
				//pas sur d'envoyer la 221 ici peut etre pas une numeric replies juste
				//mettre les modes avec MODE en nomb de commande?
				sendToClient(221); //RPL_UMODEIS 
			}
		}
		//faut rajouter une erreur si on nous envoie une option de mode qu'on gere pas
		//mais on est quand meme sencer ajotuer celle qu'on connait
		//et visiblement on peut recevoir en une commande plusieurs options genre + iw
		//donc si on recoit + zzzzzzzzzziw faut ignorer les z rajouter iw et ensutie mettre l erreur pour les z???
	}
}


// OPER
void	Command::oper()
{
	if (DEBUG)
		std::cout << "Command::oper" << std::endl;
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
	if ((*_cmd)[_actual_cmd][1] == _oper_name && (*_cmd)[_actual_cmd][2] == _oper_pass)
	{
		_client->setOper(true);
		sendToClient(381);
		//mettre aussi un message de MODE (RPL_UMODEIS)
		return ;
	}
	else
	{
		sendToClient(464);//ERR_PASSWDMISMATCH
		return ;
	}
}

// CAP
void	Command::cap()
{
	if (DEBUG)
		std::cout << "Command::cap" << std::endl;
}

// PASS
void	Command::pass()
{
	if (DEBUG)
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
		fatalError("You SHOULD connect with the good password.");
		return ;
	}
}

// NICK
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
			return (1);
	}
	return (0);
}

void	Command::nick()
{
	std::cout << "Command::nick" << std::endl;
	if (!parsingNickname((*_cmd)[_actual_cmd][1]))
	{
		sendToClient(432); //ERR_ERRONEUSNICKNAME
		return ;
	}
	else if (checkNickname((*_cmd)[_actual_cmd][1]))
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
		if (DEBUG)
			std::cout << "old nick name = " << _client->getNickname() << std::endl;
		_client->setNickname((*_cmd)[_actual_cmd][1]);
		if (DEBUG)
			std::cout << "new nick name = " << _client->getNickname() << std::endl;
		//avertir le client que la commande nick est successfull et avertir les autres clients du changement de nickname
	}
}

// USER
void	Command::user()
{
	if (DEBUG)
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
		if (DEBUG)
			std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
		_client->setUsername((*_cmd)[_actual_cmd][1]);
		if (DEBUG)
			std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
	}
	sendToClient(1);
	sendToClient(2);
	sendToClient(3);
	sendToClient(4);
}

// JOIN
void	Command::join()
{
	if ((*_cmd)[_actual_cmd].size() == 1)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	//on parcoure tous les chans
	for (std::vector<Channel *>::iterator it = _all_channels.begin() ; it != _all_channels.end() ; ++it)
	{
		//si le chan existe déjà
		if ((*it)->getName() == (*_cmd)[_actual_cmd][1])
		{
			//on ajoute le chan au client et le client au chan et on quitte la commande
			_client->addChannel(*it);
			(*it)->addClient(_client); 
			//on crée le msg de client qui join le chan
			std::string msg;
			msg = ":" + _client->getNickname() + " JOIN " + (*it)->getName() + "\r\n";
			std::vector<Client *> listClientsChan = (*it)->getListClients();
			for (std::vector<Client *>::iterator it2 = listClientsChan.begin() ; it2 != listClientsChan.end() ; ++it2)
			{
				/*on envoie le message aux autres clients du chan
				if ((*it2) != _client)
					send((*it2)->getSock(), msg.c_str(), msg.size(), 0); 
				*/
				//on envoie le message à tous les clients du chan (meme nous)
				send((*it2)->getSock(), msg.c_str(), msg.size(), 0); 
			}
			return ;
		}
	}
	//si le chan existe pas : on le crée
	Channel *new_chan = new Channel((*_cmd)[_actual_cmd][1], _client);
	//on rajoute le chan dans la liste des chans du client	
	_client->addChannel(new_chan);
	//on ajoute le nouveau chan à la liste _all_chans
	_all_channels.push_back(new_chan);

	//POUR REGARDER CE QUI A UN RAPPORT AVEC LES CHANNELS
	/*	for(std::vector<Channel *>::iterator it1 = _all_channels.begin() ; it1 != _all_channels.end() ; ++it1)
		{
		std::cout << "_all_channels = " << (*it1)->getName() << std::endl;
		}
		std::vector<Channel *> chacha = (*_client).getClientChannels();
		for(std::vector<Channel *>::iterator it2 = chacha.begin() ; it2 != chacha.end() ; ++it2)
		{
		std::cout << "_client->_client_channels = " << (*it2)->getName() << std::endl;
		}
		std::vector<Client *> clicli = new_chan->getListClients();
		for(std::vector<Client *>::iterator it3 = clicli.begin() ; it3 != clicli.end() ; ++it3)
		{
		std::cout << "new_chan->list_clients = " << (*it3)->getNickname() << std::endl;
		}*/
}

void Command::kick()
{
	if (DEBUG)
		std::cout << "Command::kick" << std::endl;
}
void Command::kill()
{
	if (DEBUG)
		std::cout << "Command::kill" << std::endl;
}
// QUIT
void	Command::quit()
{
	if (DEBUG)
		std::cout << "Command::notice" << std::endl;
	//faudra bien tout libérer ici
	//fatal_error
	_client->setStatus(REMOVE_ME);
}

//NOTICE == SAME AS PRIVMSG BUT NEVER SEND AUTOMATIC REPLY
void	Command::notice()
{
	if (DEBUG)
		std::cout << "Command::notice" << std::endl;
	if ((*_cmd)[_actual_cmd].size() < 3)
		return ;
	std::string target_name = (*_cmd)[_actual_cmd][1];
	if (target_name[0] == '#') //Channel part
	{
		for(std::vector<Channel *>::iterator it = _all_channels.begin() ; it != _all_channels.end() ; ++it)
		{
			if ((*it)->getName() == target_name)
			{
				sendToChannel((*it), true);
				return ;
			}
		}
	}
	else //Client part
	{
		std::cout << "notice nice part" << std::endl;
		for (std::map<int, Client *>::iterator it = (*_clients_ptr).begin() ; it != (*_clients_ptr).end() ; ++it)
		{
			if ((*it).second->getNickname() == target_name) 
			{
				sendToTarget(target_name, (*it).first, true);
				return ;
			}
		}
	}
}

// PRIVMSG
void	Command::privmsg()
{
	if (DEBUG)
		std::cout << "Command::privmsg" << std::endl;
	if ((*_cmd)[_actual_cmd].size() < 3)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	std::string target_name = (*_cmd)[_actual_cmd][1];
	//Channel part
	if (target_name[0] == '#')
	{
		for(std::vector<Channel *>::iterator it = _all_channels.begin() ; it != _all_channels.end() ; ++it)
		{
			if ((*it)->getName() == target_name)
			{
				_actual_chan = (*it);
				sendToChannel((*it), false);
				return ;
			}
		}
		sendToClient(404); //ERR_CANNOTSENDTOCHAN
	}
	else //CLient part
	{
		if (!checkNickname(target_name))
		{
			sendToClient(401); //ERR_NOSUCHNICK 
			return ;
		}
		for (std::map<int, Client *>::iterator it = (*_clients_ptr).begin() ; it != (*_clients_ptr).end() ; ++it)
		{
			if ((*it).second->getNickname() == target_name) 
			{
				sendToTarget(target_name, (*it).first, false);
				return ;
			}
		}
	}
	sendToClient(401); //ERR_NOSUCHNICK (401)			
}

// SEND TO
// PRIVMSG TARGET
void Command::sendToTarget(std::string target_name, int target_socket, bool is_notice)
{
	if (DEBUG)
		std::cout << "target name puis target socket " << target_name << " "<< target_socket << std::endl;
	std::string msg;
	if (is_notice)
		msg = ":" + _client->getNickname() + " NOTICE " + target_name + " " ;
	else
		msg = ":" + _client->getNickname() + " PRIVMSG " + target_name + " " ;
	for(std::vector<std::string>::iterator it2 = (*_cmd)[_actual_cmd].begin() + 2 ; it2 != (*_cmd)[_actual_cmd].end() ; ++it2)
	{
		if (it2 != (*_cmd)[_actual_cmd].begin() + 2)
			msg += " ";
		msg += *it2;
	}
	msg += "\r\n";
	if (DEBUG)
		std::cout << "MSG ="<< msg << std::endl;
	send(target_socket, msg.c_str(), msg.size(), 0);
}

// PRIVMSG CHANNEL
void Command::sendToChannel(Channel *channel, bool is_notice)
{
	std::string msg;
	if (is_notice)
		msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@localhost NOTICE " + (*channel).getName()  + " " ;
	else
		msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@localhost PRIVMSG " + (*channel).getName()  + " " ;
	for(std::vector<std::string>::iterator it = (*_cmd)[_actual_cmd].begin() + 2 ; it != (*_cmd)[_actual_cmd].end() ; ++it)
	{
		if (it != (*_cmd)[_actual_cmd].begin() + 2)
			msg += " ";
		msg += *it;
	}
	msg += "\r\n";
	if (DEBUG)
		std::cout << "MSG ="<< msg << std::endl;
	std::vector<Client *> clicli = (*channel).getListClients();
	for (std::vector<Client *>::iterator it2 = clicli.begin() ; it2 != clicli.end() ; ++it2)
	{
		if ((*it2) != _client)
			send((*it2)->getSock(), msg.c_str(), msg.size(), 0);
	}
}

std::string Command::getTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	std::string return_s(buf);

    return (return_s);
}

//NUMERIC REPLIES
void Command::sendToClient(int numeric_replies)
{
	std::string msg;

	if (numeric_replies >= 1 && numeric_replies <= 5)
		msg = ":" + _server_name + " " + insert_zeros(numeric_replies) + to_string(numeric_replies) + " " + _client->getNickname() + " :";
	else
		msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " " + insert_zeros(numeric_replies) + to_string(numeric_replies) + " :";
	switch (numeric_replies)
	{
		case 1: //RPL_WELCOME
			{			
				msg += "Welcome to the Internet Relay Network " + _client->getNickname() + "\r\n";
				break;
			}
		case 2:
			{
				msg += "Your host is " + _server_name + ", running on version [ft_irc]\r\n";
				break;
			}
		case 3:
			{
				msg += "This server was created at: " + _creationTime + " \r\n";
				break;
			}
		case 4:
			{
				msg += _server_name + " version [ft_irc]. Available user MODE : +o . Avalaible channel MODE : none. \r\n";
				break;
			}
		case 5:
			{
				msg += "CHANTYPES=# CHANMODES=,,,, MODES=1 :are supported by this server\r\n";
				break;
			}
		case 221: //RPL_UMODEIS
			{
				msg += _client->getUsername() + _client->getOptions() + "\r\n";	
				break ;
			}
		case 381: //RPL_YOUREOPER
			{
				msg += _client->getUsername() + " :You are now an IRC operator\r\n";
				break;
			}
		case 401: //ERR_NOSUCHNICK
			{
				msg += _client->getUsername() + " " + _client->getNickname() + " :No such nick/channel\r\n";
				break;
			}
		case 402: //ERR_NOSERVER
			{
				msg += _client->getUsername() + " " + _server_name + " :No such server\r\n";
				break;
			}
		case 403: //ERR_NOSUCHCHANNEL //A FINIR AVEC LES CHANS
			{
				msg += _client->getUsername() + " " + " :No such channel\r\n";	
				break;
			}
		case 404: //ERR_CANNOTSENDTOCHAN // A FINIR AVEC LES CHANS
			{
				msg += _client->getUsername() + " " + " :Cannot send to channel\r\n";
				break;
			}
		case 405: //ERR_TOOMANYCHANNELS // A FINIR AVEC LES CHANS
			{
				msg += _client->getUsername() + " " + " :You have  joined too many channels\r\n";
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
		case 471: //ERR_CHANNELISFULL // A FINIR AVEC LES CHANS
			{
				msg += _client->getUsername() + " " + " :Cannot join channel (+1)\r\n";	
				break;
			}
		case 473: //ERR_INVITEONLYCHAN // A FINIR AVEC LES CHANS
			{
				msg += _client->getUsername() + " " + " :Cannot join channel (+i)\r\n";	
				break;
			}
		case 474: //ERR_BANNEDFROMCHAN // A FINIR AVEC LES CHANS
			{
				msg += _client->getUsername() + " " + " :Cannot join channel (+b)\r\n";	
				break;
			}
		case 475: //ERR_BADCHANNELKEY // A FINIR AVEC LES CHANS
			{
				msg += _client->getUsername() + " " + " :Cannot join channel (+k)\r\n";	
				break;
			}
		case 476: //ERR_BADCHANMASK // A FINIR AVEC LES CHANS
			{
				msg += " :Bad Channel Mask\r\n";	
				break;
			}
		case 482: //ERR_CHANOPRIVSNEEDED
			{
				msg += _client->getUsername() + _actual_chan->getName() +" :You're not channel operator\r\n";	
				break;
			}
		case 491: //ERR_NOOPERHOST
			{
				msg += _client->getUsername() + " :No O-lines for your host\r\n";	
				break;
			}
		default :
			{
				msg = "ON EST DANS LE DEFAULT DE SENDTOCLIENT\r\n";
				break ;
			}

	}
	if (DEBUG)
		std::cout << msg << std::endl;
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

// ERROR
void Command::fatalError(std::string msg_error)
{
	std::string msg;
	msg = ":" + _client->getNickname() + " " + "ERROR" + " :" +msg_error + "\r\n";
	if (DEBUG)
		std::cout << msg << std::endl;

	send(_client_socket, msg.c_str(), msg.size(), 0);
	_fatal_error = true;
	close(_client_socket);
	(*_clients_ptr).erase(_client_socket);
	std::cout << "fatalError" << std::endl;
}

// CHANNELS PART

std::vector<Channel *> Command::getAllChannels()
{
	return (_all_channels);
}
