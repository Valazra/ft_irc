#include "Channel.hpp"

Channel::Channel(std::string name_chan, Client *client) : _name(name_chan), _list_clients(1, client), _chan_operator(client), _has_topic(false)
{
}

Channel::~Channel()
{
}

std::string Channel::getName()
{
	return (_name);
}

std::vector<Client *> *Channel::getListClients()
{
	return (&_list_clients);
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
	_creation_time_topic = getTime();
}

void Channel::setHasTopicOff()
{
	_has_topic = false;
}

void Channel::deleteTopic()
{
	_topic.clear();
	_creation_time_topic.clear();
}

std::string Channel::getCreationTimeTopic()
{
	return (_creation_time_topic);
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

std::string Channel::getTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	std::string return_s(buf);

	return (return_s);
}
