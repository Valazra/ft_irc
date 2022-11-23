/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jholl <jholl@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/23 18:37:40 by jholl             #+#    #+#             */
/*   Updated: 2022/11/24 00:15:05 by jholl            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

Command::Command()
{
}

Command::~Command()
{
}

Command::Command(Client *client):
_client(client)
{
	_cmd_availables["CAP"] = &Command::cap;
	_cmd_availables["NICK"] = &Command::nick;
	_cmd_availables["USER"] = &Command::user;
}



void	Command::registerAttempt()
{
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

void Command::whoAmI()
{
	if (_client->getStatus() == TO_REGISTER)
	{
		registerAttempt();
	}
}

void	Command::cap(CMD_ARGS)
{
	(void)cmd;
}
void	Command::nick(CMD_ARGS)
{
	(void)cmd;
}
void	Command::user(CMD_ARGS)
{
	(void)cmd;
}
