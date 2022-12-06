#include "Command.hpp"

Command::Command(std::map<int, Client *> *client_map, std::string password, bool *fatal_error):
	_clients_ptr(client_map), _password(password), _correctPass(false), _server_name("localhost"), _oper_name("coco"), _oper_pass("toto"), _bad_chan_name(), _bad_chan_bool(false), _bad_nickname(), _fatal_error(fatal_error), _creationTime(getTime()) 
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
	_cmd_list.push_back("kill");
	_cmd_list.push_back("KICK");
	_cmd_list.push_back("TOPIC");
	_cmd_list.push_back("INVITE");
	_cmd_list.push_back("PART");
	_cmd_list.push_back("NAMES");
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
	_cmd_availables["kill"] = &Command::kill;
	_cmd_availables["KICK"] = &Command::kick;
	_cmd_availables["TOPIC"] = &Command::topic;
	_cmd_availables["INVITE"] = &Command::invite;
	_cmd_availables["PART"] = &Command::part;
	_cmd_availables["NAMES"] = &Command::names;
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
		if (DEBUG)
			std::cout << "execCmd() this cmd:" << it->front() << std::endl;
		_bad_chan_bool = false;
		_bad_chan_name.clear();
		if (!check_if_valid_cmd(it->front()))
		{
			sendToClient(421); //ERR_UNKNOWNCOMMAND
			return ;
		}
		(this->*_cmd_availables[it->front()])();
		if (*_fatal_error)
			return ;
		_actual_cmd++;
	}
}

void Command::readCmd(int client_socket)
{
	*_fatal_error = false;
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
	_cmd_availables["TOPIC"] = &Command::topic;
	if ((*_cmd)[_actual_cmd][1][0] == '#')
	{
		//Channel part
		for(std::vector<Channel *>::iterator it = (_all_channels).begin() ; it != (_all_channels).end() ; ++it)
		{
			if ((*it)->getName() == target_name)
			{
				_actual_chan = (*it);
				if ((*_cmd)[_actual_cmd].size() == 2)
				{
					sendToClient(324); //RPL_CHANNELMODEIS
					return ;
				}
				sendToClient(477); //ERR_NOCHANMODES
				return;
			}
		}
		_bad_chan_name = target_name; 
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
		if ((*_cmd)[_actual_cmd][2][0] == '-')
		{
			if ((*_cmd)[_actual_cmd][2].size() == 2 && (*_cmd)[_actual_cmd][2][1] == 'o')
			{
				(*_client).setOper(false);
				sendToClient(221); //RPL_UMODEIS 
				return ;
			}
		}
		sendToClient(501); //ERR_UMODEUNKNOWNFLAG
	}
}

int	Command::nbCommas(std::string chans)
{
	int nb_commas = 0;
	for(std::string::iterator it = chans.begin() ; it != chans.end() ; ++it)
	{
		if ((*it) == ',')
			nb_commas++;
	}
	return (nb_commas);
}

std::vector<std::string> Command::splitCommas(std::string listchans)
{
	std::vector<std::string> vect_chan;
	std::string tmp;
	int i = 0;
	for(std::string::iterator it = listchans.begin() ; it != listchans.end() ; ++it)
	{
		if (tmp[0] == ',')
		{
			tmp.erase(0, 1);
			i = 0;
		}
		if ((*it) == ',')
		{
			vect_chan.push_back(tmp);
			tmp.clear();
			i = 0;
		}
		tmp.push_back((*it));
		i++;
	}
	if (tmp.size() > 0)
		vect_chan.push_back(tmp);
	return (vect_chan);
}

std::vector<Channel *>::iterator Command::returnItChan(std::string chan_name)
{
	for(std::vector<Channel *>::iterator it = (_all_channels).begin() ; it != (_all_channels).end() ; ++it)
	{
		if ((*it)->getName() == chan_name)
			return (it);
	}
	return (_all_channels.end());
}

void 	Command::checkIfEmptyChan()
{
	std::vector<Channel *> tmp;
	for(std::vector<Channel *>::iterator it = (_all_channels).begin() ; it != (_all_channels).end() ; ++it)
	{
		if ((*it)->getListClients()->size() == 0)
			tmp.push_back(*it);
	}
	for(std::vector<Channel *>::iterator it = tmp.begin() ; it != tmp.end() ; ++it)
	{
			(_all_channels).erase(returnItChan((*it)->getName()));
			delete ((*it));
	}
}

// PART
void	Command::part()
{
	if (DEBUG)
		std::cout << "Command::part" << std::endl;
	if ((*_cmd)[_actual_cmd].size() == 1)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	int nb_commas = nbCommas((*_cmd)[_actual_cmd][1]);
	if (nb_commas)
	{
		bool do_error = 1;
		std::vector<std::string> vect_chan = splitCommas((*_cmd)[_actual_cmd][1]);
		for (std::vector<std::string>::iterator it = vect_chan.begin() ; it != vect_chan.end() ; ++it)
		{
			do_error = true;
			//on parcoure tous les chans
			for (std::vector<Channel *>::iterator it2 = (_all_channels).begin() ; it2 != (_all_channels).end() ; ++it2)
			{
				//si le chan existe
				if ((*it2)->getName() == (*it))
				{
					_actual_chan = (*it2);
					std::vector<Client *> *listClients = (*it2)->getListClients();
					//on parcoure tous les clients du chan
					for(std::vector<Client *>::iterator it3 = listClients->begin() ; it3 != listClients->end() ; ++it3)
					{
						//si le client est dans le chan
						if ((*it3)->getNickname() == _client->getNickname())
						{
							_client->leaveChannel(*it2);
							(*it2)->deleteClient(*it3);
							checkIfEmptyChan();
							std::string msg;
							msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " PART " + (*_cmd)[_actual_cmd][1];
							if ((*_cmd)[_actual_cmd].size() > 2)
							{
								std::string reason;
								for(std::vector<std::string>::iterator it4 = (*_cmd)[_actual_cmd].begin() + 2 ; it4 != (*_cmd)[_actual_cmd].end() ; ++it4)
								{
									reason += " ";
									reason += *it4;
								}
								msg += reason;
							}
							msg += "\r\n";
							std::cout << "MSG = " << msg << std::endl;
							for (std::vector<Client *>::iterator it4 = listClients->begin() ; it4 != listClients->end() ; ++it4)
								send((*it4)->getSock(), msg.c_str(), msg.size(), 0);
							send(_client->getSock(), msg.c_str(), msg.size(), 0);
							do_error = false;
							break ;
						}
					}
					//si le client est pas dans le chan
					if (do_error)
					{
						sendToClient(442); //ERR_NOTONCHANNEL
						do_error = false;
					}
				}
			}
			//si on a pas trouvé le chan
			if (do_error)
			{
				_bad_chan_name = (*it);
				sendToClient(403); //ERR_NOSUCHCHANNEL
				do_error = false;
			}
		}
	}
	else
	{
		//on parcoure tous les chans
		for (std::vector<Channel *>::iterator it = (_all_channels).begin() ; it != (_all_channels).end() ; ++it)
		{
			//si le chan existe
			if ((*it)->getName() == (*_cmd)[_actual_cmd][1])
			{
				_actual_chan = (*it);
				std::vector<Client *> *listClients = (*it)->getListClients();
				//on parcoure tous les clients du chan
				for(std::vector<Client *>::iterator it2 = listClients->begin() ; it2 != listClients->end() ; ++it2)
				{
					//si le client est dans le chan
					if ((*it2)->getNickname() == _client->getNickname())
					{
						_client->leaveChannel(*it);
						(*it)->deleteClient(*it2);
						std::string msg;
						msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " PART " + (*_cmd)[_actual_cmd][1];
						if ((*_cmd)[_actual_cmd].size() > 2)
						{
							std::string reason;
							for(std::vector<std::string>::iterator it3 = (*_cmd)[_actual_cmd].begin() + 2 ; it3 != (*_cmd)[_actual_cmd].end() ; ++it3)
							{
								reason += " ";
								reason += *it3;
							}
							msg += reason;
						}
						msg += "\r\n";
						std::cout << "MSG = " << msg << std::endl;
						for (std::vector<Client *>::iterator it3 = listClients->begin() ; it3 != listClients->end() ; ++it3)
							send((*it3)->getSock(), msg.c_str(), msg.size(), 0);
						send(_client->getSock(), msg.c_str(), msg.size(), 0);	
						checkIfEmptyChan();
						return ;
					}
				}
				//si le client est pas dans le chan
				sendToClient(442); //ERR_NOTONCHANNEL
				return ;		
			}
		}
		//si on a pas trouvé le chan
		_bad_chan_name = (*_cmd)[_actual_cmd][1];
		sendToClient(403); //ERR_NOSUCHCHANNEL
		return ;	
	}
}

// INVITE
void	Command::invite()
{
	if (DEBUG)
		std::cout << "Command::invite" << std::endl;
	if ((*_cmd)[_actual_cmd].size() != 3) //si on en a 6 on envoi need more params, c est fifou?
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	std::string target_name = (*_cmd)[_actual_cmd][1];
	std::string name = (*_cmd)[_actual_cmd][2];
	Channel *finded_chan = findChan(name);
	_bad_chan_name = name;
	_actual_chan = finded_chan;
	if (!finded_chan)
	{
		sendToClient(403); //ERR_NOSUCHCHANNEL
		return ;
	}
	std::vector<Client *> *listClients = finded_chan->getListClients();
	for (std::vector<Client *>::iterator it = listClients->begin() ; it != listClients->end() ; ++it)
	{
		if ((*it)->getNickname() == _client->getNickname())
		{
			for (std::vector<Client *>::iterator it1 = listClients->begin() ; it1 != listClients->end() ; ++it1)
			{
				if ((*it1)->getNickname() == target_name) //is already on chan?
				{
					sendToClient(443); //ERR_USERONCHANNEL
					return ;
				}
			}
			for (std::map<int, Client *>::iterator it1 = (*_clients_ptr).begin() ; it1 != (*_clients_ptr).end() ; ++it1) 
			{
				if ((*it1).second->getNickname() == target_name) //is target existing?
				{
					std::string msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " INVITE " + target_name + " " + _actual_chan->getName() + "\r\n";
					if (DEBUG)
						std::cout << "MSG = " << msg << std::endl;
					send((*it1).first, msg.c_str(), msg.size(), 0);	
					sendToClient(341); //RPL_INVITING
					return ;
				}
			}
			sendToClient(401); //ERR_NOSUCHNICK
			return ;
		}
	}
	sendToClient(442); //ERR_NOTONCHANNEL
	return ;		
}

// TOPIC
void	Command::topic()
{
	if (DEBUG)
		std::cout << "Command::topic" << std::endl;
	if ((*_cmd)[_actual_cmd].size() == 1)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	std::string chan_name = (*_cmd)[_actual_cmd][1];
	Channel *finded_chan = findChan(chan_name);
	_bad_chan_name = (*_cmd)[_actual_cmd][1];
	_actual_chan = finded_chan;
	if ((*_cmd)[_actual_cmd].size() == 2) 
	{
		if (!finded_chan)
		{
			sendToClient(403); //ERR_NOSUCHCHANNEL
			return ;
		}
		std::vector<Client *> *listClients = finded_chan->getListClients();
		for(std::vector<Client *>::iterator it2 = listClients->begin() ; it2 != listClients->end() ; ++it2)
		{
			if ((*it2)->getNickname() == _client->getNickname())
			{
				if (!(finded_chan->getHasTopic()))
				{
					sendToClient(331); //RPL_NOTOPIC
					return ;
				}
				else
				{
					sendToClient(332); //RPL_TOPIC
					sendToClient(333); //RPL_TOPICWHOTIME
					return ;
				}
			}
		}
		sendToClient(442); //ERR_NOTONCHANNEL
		return ;
	}
	else if ((*_cmd)[_actual_cmd].size() >= 3)
	{
		if (!finded_chan)
		{
			sendToClient(403); //ERR_NOSUCHCHANNEL
			return ;
		}
		std::vector<Client *> *listClients2 = finded_chan->getListClients();
		for (std::vector<Client *>::iterator it2 = listClients2->begin() ; it2 != listClients2->end() ; ++it2)
		{
			if ((*it2)->getNickname() == _client->getNickname())
			{
				std::string topic;
				for(std::vector<std::string>::iterator it3 = (*_cmd)[_actual_cmd].begin() + 2 ; it3 != (*_cmd)[_actual_cmd].end() ; ++it3)
				{
					if (it3 != (*_cmd)[_actual_cmd].begin() + 2)
						topic += " ";
					else
					{
						std::string tmp;
						tmp = (*it3);
						tmp.erase(0,1);
						(*it3) = tmp;
					}
					topic += *it3;
				}
				if (topic == "\"\"") // "" irssi send empty string
				{
					finded_chan->deleteTopic();
					finded_chan->setHasTopicOff();
				}
				else
				{
					finded_chan->setTopic(topic);
					finded_chan->setHasTopicOn();
				}
				std::string msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " TOPIC " + _actual_chan->getName() + " :" + _actual_chan->getTopic() + "\r\n";
				for (std::vector<Client *>::iterator it4 = listClients2->begin() ; it4 != listClients2->end() ; ++it4)
					send((*it4)->getSock(), msg.c_str(), msg.size(), 0);
				return ;
			}
		}
		sendToClient(442); //ERR_NOTONCHANNEL
		return ;
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
		sendToClient(381); // 
		sendToClient(221); //RPL_UMODEIS 
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
		std::cout << "Command::pass | Pass attendu = " << _password << " | Pass recu = " << (*_cmd)[_actual_cmd][1] << std::endl;
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
	if (nickname.size() > 10)
		return (0);
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
	if (DEBUG)
		std::cout << "Command::nick" << std::endl;
	if (!parsingNickname((*_cmd)[_actual_cmd][1]) || (*_cmd)[_actual_cmd].size() > 2)
	{
		sendToClient(432); //ERR_ERRONEUSNICKNAME
		return ;
	}
	else if (checkNickname((*_cmd)[_actual_cmd][1]))
	{
		sendToClient(433); //ERR_NICKNAMEINUSE
		return ;
	}
	else if ((*_cmd)[_actual_cmd].size() == 1)
	{
		sendToClient(431); //ERR_NONICKNAMEGIVEN
		return ;
	}
	else
	{
		std::string msg;
		msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " NICK " + (*_cmd)[_actual_cmd][1] + "\r\n";
		std::vector<int> has_been_send;
		bool go_send;
		for(std::vector<Channel *>::iterator it = (_client->getClientChannels())->begin(); it != (_client->getClientChannels())->end() ; ++it)
		{
			for(std::vector<Client *>::iterator it2 = ((*it)->getListClients())->begin(); it2 != ((*it)->getListClients())->end() ; ++it2)
			{
				go_send = 1;
				for(std::vector<int>::iterator it3 = has_been_send.begin() ; it3 != has_been_send.end() ; ++it3)
				{
					if ((*it2)->getSock() == (*it3))
						go_send = 0;
				}
				if (go_send)
				{
					if ((*it2)->getSock() != _client->getSock())
					{
						send((*it2)->getSock(), msg.c_str(), msg.size(), 0);
						has_been_send.push_back((*it2)->getSock());
					}
				}
			}
		}
		send(_client->getSock(), msg.c_str(), msg.size(), 0);
		_client->setNickname((*_cmd)[_actual_cmd][1]);
	}
}

int Command::parsingRealname(std::string realname)
{
	if (realname[0] != ':')
		return (0);
	return (1);
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
	else if ((*_cmd)[_actual_cmd].size() < 4)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	else if (!parsingRealname((*_cmd)[_actual_cmd][4]))
	{
		return ; //pas de ":" devant le real name 
	}
	else
	{ 
		/*if (DEBUG)
		  {
		  std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
		  std::cout << "_client->getRealname() = " << _client->getRealname() << std::endl;
		  }*/
		_client->setUsername((*_cmd)[_actual_cmd][1]);
		_client->setRealname((*_cmd)[_actual_cmd][4]);
		/*	if (DEBUG)
			{
			std::cout << "_client->getUsername() = " << _client->getUsername() << std::endl;
			std::cout << "_client->getRealname() = " << _client->getRealname() << std::endl;
			}*/
	}
	_client->setStatus(REGISTER);
	sendToClient(1);
	sendToClient(2);
	sendToClient(3);
	sendToClient(4);
}

// NAMES
void	Command::names()
{
	if (DEBUG)
		std::cout << "Command::names" << std::endl;
	std::string chan_name;
	Channel *finded_chan;
	std::vector<std::string> vect_chan = splitCommas((*_cmd)[_actual_cmd][1]);
	for (std::vector<std::string>::iterator it = vect_chan.begin() ; it != vect_chan.end() ; ++it)
	{
		_bad_chan_bool = false;
		chan_name = (*it);
		finded_chan = findChan(chan_name);
		if (!finded_chan)
		{
			_bad_chan_bool = true;
			_bad_chan_name = (*it);
			sendToClient(366); //RPL_ENDOFNAMES;
		}
		else
		{
			_actual_chan = finded_chan;
			nameReply(chan_name, finded_chan);
		}
	}
}

// JOIN
Channel *Command::findChan(std::string chan_name)
{
	if ((_all_channels).empty())
		return (NULL);
	for (std::vector<Channel *>::iterator it = (_all_channels).begin() ; it != (_all_channels).end() ; ++it)
		if ((*it)->getName() == chan_name)
			return (*it);
	return (NULL);
}

void	Command::msgJoin(std::string chan_name, Channel *finded_chan)
{
	std::string msg;
	msg =":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " " + " JOIN " + chan_name + "\r\n";
	std::vector<Client *> *listClientsChan = finded_chan->getListClients();
	for (std::vector<Client *>::iterator it = listClientsChan->begin() ; it != listClientsChan->end() ; ++it)
		send((*it)->getSock(), msg.c_str(), msg.size(), 0);
	if (finded_chan->getHasTopic() == true)
		sendToClient(332); //RPL_TOPIC
	nameReply(chan_name, finded_chan);

}
void	Command::join()
{
	if ((*_cmd)[_actual_cmd].size() == 1)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	std::string chan_name = (*_cmd)[_actual_cmd][1];
	Channel *finded_chan = findChan(chan_name);
	if (!finded_chan) //No chan We create it
	{
		Channel *new_chan = new Channel(chan_name, _client);
		_client->addChannel(new_chan);
		(_all_channels).push_back(new_chan);
		_actual_chan = new_chan;
		msgJoin(chan_name, new_chan);
	}
	else //Chan already exist
	{
		_client->addChannel(finded_chan);
		(finded_chan)->addClient(_client); 
		_actual_chan = finded_chan;
		msgJoin(chan_name, finded_chan);
	}
}

// NAME REPLY
void Command::nameReply(std::string chan_name, Channel *chan)
{
	std::string msg;
	msg =":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " 353 ";
	msg += _client->getUsername() + " = " + chan_name + " :";
	for (std::vector<Client *>::iterator it = (chan->getListClients())->begin() ; it != (chan->getListClients())->end() ; ++it)
	{
		if ((*it)->getNickname() == (chan->getChannelOperator()->getNickname()))
			msg += "@"; 
		msg += (*it)->getNickname();
		if ((it + 1) != chan->getListClients()->end()) 
			msg += " ";
	}
	msg += "\r\n";
	send(_client_socket, msg.c_str(), msg.size(), 0);
	sendToClient(366); //RPL_ENDOFNAMES
}

// KICK
//gérer le cas où on se kick soi meme et gérer le cas ou on kick plusieurs targets en meme temps
void Command::kick()
{
	if (DEBUG)
		std::cout << "Command::kick" << std::endl;
	if ((*_cmd)[_actual_cmd].size() < 3)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	std::string chan_name = (*_cmd)[_actual_cmd][1];
	Channel *finded_chan = findChan(chan_name);
	_bad_chan_name = (*_cmd)[_actual_cmd][1];
	_actual_chan = finded_chan;
	if (!finded_chan)
	{
		sendToClient(403); //ERR_NOSUCHCHANNEL
		return ;
	}
	std::vector<Client *> *listClients = finded_chan->getListClients();
	for(std::vector<Client *>::iterator it = listClients->begin() ; it != listClients->end() ; ++it)
	{
		if ((*it)->getNickname() == _client->getNickname())
		{
			for(std::vector<Client *>::iterator it3 = listClients->begin() ; it3 != listClients->end() ; ++it3)
			{
				if ((*it3)->getNickname() == (*_cmd)[_actual_cmd][2])
				{
					if (finded_chan->getChannelOperator() == _client)
					{
						std::string msg;
						msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " KICK " + _actual_chan->getName() + " " + (*it3)->getNickname();
						if ((*_cmd)[_actual_cmd].size() > 3)
							for(std::vector<std::string>::iterator it4 = (*_cmd)[_actual_cmd].begin() + 3 ; it4 != (*_cmd)[_actual_cmd].end() ; ++it4)
								msg += " " + (*it4);
						msg += "\r\n";
						if (DEBUG)
							std::cout << "MSG KICK = " << msg << std::endl;
						for (std::vector<Client *>::iterator it5 = listClients->begin() ; it5 != listClients->end() ; ++it5)
							send((*it5)->getSock(), msg.c_str(), msg.size(), 0);
						finded_chan->deleteClient(*it3);
						(*it3)->leaveChannel(finded_chan);
						checkIfEmptyChan();
						return ;
					}
					sendToClient(482); //ERR_CHANOPRIVSNEEDED
					return ;
				}
			}
			_bad_nickname = (*_cmd)[_actual_cmd][2];
			sendToClient(441); //ERR_USERNOTINCHANNEL
			_bad_nickname.clear();
			return ;
		}
	}
	sendToClient(442); //ERR_NOTONCHANNEL
	return ;
}

// KILL
/*
   When a client is removed as the result of a KILL message, the server
   SHOULD add the nickname to the list of unavailable nicknames in an
   attempt to avoid clients to reuse this name immediately which is
   usually the pattern of abusive behaviour often leading to useless
   "KILL loops".
 */
void Command::kill()
{
	if (DEBUG)
		std::cout << "Command::kill" << std::endl;
	if ((*_cmd)[_actual_cmd].size() < 3)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	if (!(*_client).getOper())
	{
		sendToClient(481); //ERR_NOPRIVILEGES
		return ;
	}
	if (!checkNickname((*_cmd)[_actual_cmd][1]))
	{
		sendToClient(401); //ERR_NOSUCHNICK 
		return ;
	}
	int socket_killed;
	for (std::map<int, Client *>::iterator it = (*_clients_ptr).begin() ; it != (*_clients_ptr).end() ; ++it)
	{
		if ((*it).second->getNickname() == (*_cmd)[_actual_cmd][1])
		{
			socket_killed = (*it).first;
		}
	}
	std::string reason_of_kill;
	for(std::vector<std::string>::iterator it = (*_cmd)[_actual_cmd].begin() + 2 ; it != (*_cmd)[_actual_cmd].end() ; ++it)
	{
		if (it != (*_cmd)[_actual_cmd].begin() + 2)
			reason_of_kill += " ";
		reason_of_kill += *it;
	}
//////////////////////TOUT SUR LES CHANS)
	std::string msg;
	msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name;
	msg += " KILL " + (*_cmd)[_actual_cmd][1] + " :" + reason_of_kill + "\r\n";
	send(socket_killed, msg.c_str(), msg.size(), 0);

	msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name;
	msg += " QUIT " + (*_cmd)[_actual_cmd][1] + " :";
	msg += "Killed (" + _client->getNickname() + "(" + reason_of_kill + "))\r\n";
	send(socket_killed, msg.c_str(), msg.size(), 0);

	msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name;
	msg += " ERROR :Closing Link: " + _server_name;
	msg += "(Killed (" + _client->getNickname() + "(" + reason_of_kill + ")))\r\n";
	send(socket_killed, msg.c_str(), msg.size(), 0);
	closeConnection(socket_killed);
	*_fatal_error = true;
}

// QUIT
void	Command::quit()
{
	if (DEBUG)
		std::cout << "Command::quit" << std::endl;
	std::string msg;
	std::string reason;
	msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " QUIT :";
	if ((*_cmd)[_actual_cmd].size() == 2 && (*_cmd)[_actual_cmd][1] == ":leaving")
	{
		reason = "Quit: ";
		msg += reason;
	}
	else
	{
		reason = "Quit";
		for(std::vector<std::string>::iterator it = (*_cmd)[_actual_cmd].begin() + 1 ; it != (*_cmd)[_actual_cmd].end() ; ++it)
		{
			if (it != (*_cmd)[_actual_cmd].begin() + 1)
				reason += " ";
			else
			{
				std::string tmp;
				tmp = (*it);
				tmp.insert(1, " ");
				(*it) = tmp;
			}
			reason += *it;
		}
		msg += reason;
	}
	msg += "\r\n";
	std::cout << "msg = " << msg << std::endl;
	//on parcoure tous les chans du client
	for (std::vector<Channel *>::iterator it = (_client->getClientChannels())->begin() ; it != (_client->getClientChannels())->end() ; ++it)
	{
		//on parcoure tous les clients du chan
		for(std::vector<Client *>::iterator it2 = ((*it)->getListClients())->begin() ; it2 != ((*it)->getListClients())->end() ; ++it2)
		{
			if ((*it2) != _client)
				send((*it2)->getSock(), msg.c_str(), msg.size(), 0);
		}
	}
	fatalError(msg);
}

// NOTICE == SAME AS privmsg BUT NEVER SEND AUTOMATIC REPLY
void	Command::notice()
{
	if (DEBUG)
		std::cout << "Command::notice" << std::endl;
	if ((*_cmd)[_actual_cmd].size() < 3)
		return ;
	std::string target_name = (*_cmd)[_actual_cmd][1];
	if (target_name[0] == '#') //Channel part
	{
		for(std::vector<Channel *>::iterator it = (_all_channels).begin() ; it != (_all_channels).end() ; ++it)
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
	if ((*_cmd)[_actual_cmd].size() == 1)
	{
		sendToClient(461); //ERR_NEEDMOREPARAMS
		return ;
	}
	else if ((*_cmd)[_actual_cmd].size() == 2)
	{
		sendToClient(412);
		return ;
	}
	std::string target_name = (*_cmd)[_actual_cmd][1];
	//Channel part
	if (target_name[0] == '#')
	{
		for(std::vector<Channel *>::iterator it = (_all_channels).begin() ; it != (_all_channels).end() ; ++it)
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
		msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " NOTICE " + target_name + " " ;
	else
		msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " PRIVMSG " + target_name + " " ;
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
	std::vector<Client *> *clicli = (*channel).getListClients();
	for (std::vector<Client *>::iterator it2 = clicli->begin() ; it2 != clicli->end() ; ++it2)
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
		msg = ":" + _client->getNickname() + "!" + _client->getUsername() + "@" + _server_name + " " + insert_zeros(numeric_replies) + to_string(numeric_replies) + " ";
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
				msg += "TARGMAX=1 CHANTYPES=# CHANMODES=,,,, MODES=1 :are supported by this server\r\n";
				break;
			}
		case 221: //RPL_UMODEIS
			{
				msg += _client->getUsername() + _client->getOptions() + "\r\n";	
				break ;
			}
		case 324: //RPL_CHANNELMODEIS
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " \r\n"; //if modes are implements they are after chan name	
				break ;
			}
		case 331: //RPL_NOTOPIC
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :No topic is set\r\n";
				break;
			}
		case 332: //RPL_TOPIC
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :" + _actual_chan->getTopic() + "\r\n";
				break;
			}
		case 333: //RPL_TOPICWHOTIME
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " " + _client->getNickname() + " " + _actual_chan->getCreationTimeTopic() + "\r\n";
				break;
			}
		case 341: //RPL_INVITING
			{
				msg += _client->getUsername() + " " + (*_cmd)[_actual_cmd][1] + " " + _actual_chan->getName() + "\r\n";
				break;
			}
		case 366:
			{
				if (_bad_chan_bool)
					msg += _client->getUsername() + " " + _bad_chan_name + " :End of /NAMES list\r\n";	
				else
					msg += _client->getUsername() + " " + _actual_chan->getName() + " :End of /NAMES list\r\n";	
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
		case 403: //ERR_NOSUCHCHANNEL
			{
				msg += _client->getUsername() + " " + _bad_chan_name + " :No such channel\r\n";
				break;
			}
		case 404: //ERR_CANNOTSENDTOCHAN
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :Cannot send to channel\r\n";
				break;
			}
		case 405: //ERR_TOOMANYCHANNELS
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :You have  joined too many channels\r\n";
				break;
			}
		case 412: //ERR_NOTEXTTOSEND
			{
				msg += ":No text to send\r\n";
				break;
			}
		case 431: //ERR_NONICKNAMEGIVEN
			{
				msg += " :No nickname given\r\n";	
				break;
			}
		case 421: //ERR_UNKNOWNCOMMAND
			{
				msg += _client->getUsername() + " " + (*_cmd)[_actual_cmd][0] + " :Unknown command\r\n";
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
		case 441: //ERR_USERNOTINCHANNEL
			{
				msg += _client->getUsername() + " " + _bad_nickname + " " + _actual_chan->getName() + " :They aren't on that channel\r\n";
				break;
			}
		case 442: //ERR_NOTONCHANNEL
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :You're not on that channel\r\n";
				break;
			}
		case 443: //ERR_USERONCHANNEL
			{
				msg += _client->getUsername() + " " + _client->getNickname() + " " + _actual_chan->getName() + " :is already on channel\r\n";
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
		case 471: //ERR_CHANNELISFULL
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :Cannot join channel (+1)\r\n";	
				break;
			}
		case 473: //ERR_INVITEONLYCHAN
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :Cannot join channel (+i)\r\n";	
				break;
			}
		case 474: //ERR_BANNEDFROMCHAN
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :Cannot join channel (+b)\r\n";	
				break;
			}
		case 475: //ERR_BADCHANNELKEY
			{
				msg += _client->getUsername() + " " + _actual_chan->getName() + " :Cannot join channel (+k)\r\n";	
				break;
			}
		case 476: //ERR_BADCHANMASK
			{
				msg += _actual_chan->getName() + " :Bad Channel Mask\r\n";	
				break;
			}
		case 477: //ERR_NOCHANMODES
			{
				msg += _actual_chan->getName() + " :Channel doesn't support modes\r\n";	
				break;
			}
		case 481: //ERR_NOPRIVILEGES
			{
				msg += _client->getUsername() + " :Permission Denied- You're not an IRC operator\r\n";
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
		case 501: //ERR_UMODEUNKNOWNFLAG
			{
				msg += _client->getUsername() + " :Unknown MODE flag\r\n";	
				break;
			}
		case 502: //ERR_USERSDONTMATCH 
			{
				msg += _client->getUsername() + " :Cannot change mode for other users\r\n";	
				break;
			}
		default :
			{
				msg = "DEFAULT CASE\r\n";
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
	msg = ":" + _client->getNickname() + " " + "ERROR" + " " + msg_error + "\r\n";
	if (DEBUG)
		std::cout << msg << std::endl;
	send(_client_socket, msg.c_str(), msg.size(), 0);
	*_fatal_error = true;
	closeConnection(_client_socket);
}

void Command::closeConnection(int close_socket)
{
	//before check if user in channel
	close(close_socket);
	//free?
	(*_clients_ptr).erase(close_socket);
}

// CHANNELS PART
std::vector<Channel *> Command::getAllChannels()
{
	return (_all_channels);
}
