#include "Channel.hpp"

Channel::Channel(std::string name_chan, Client *client) : _name(name_chan), _list_clients()
{
	//on ajoute à la liste des clients de ce chan le client qui le crée
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
