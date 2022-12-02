#include "Channel.hpp"

Channel::Channel(std::string name_chan, Client *client) : _name(name_chan), _list_clients(), _chan_operator(client), _has_topic(false)
{
	_list_clients.push_back(client);
}

Channel::~Channel()
{
}

std::string Channel::getName()
{
	return (_name);
}

std::vector<Client *> Channel::getListClients()
{
	return (_list_clients);
}

Client *Channel::getChannelOperator()
{
	return (_chan_operator);
}

std::string Channel::getTopic()
{
	return (_topic);
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

bool Channel::getHasTopic()
{
	return (_has_topic);
}

void Channel::setHasTopicOn()
{
	_has_topic = true;
}

void Channel::setHasTopicOff()
{
	_has_topic = false;
}

void Channel::addClient(Client *client)
{
	_list_clients.push_back(client);
}

void Channel::deleteClient(Client *client)
{
	for (std::vector<Client *>::iterator it = _list_clients.begin() ; it != _list_clients.end() ; ++it)
	{
		if ((*it) == client)
		{
			_list_clients.erase(it);
			return ;
		}
	}
}
