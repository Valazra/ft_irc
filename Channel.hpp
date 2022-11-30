#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include <iostream>
# include <vector>

class Client;
class Command;

class Channel
{
	public:
		Channel(std::string name_chan, Client *client);
		virtual ~Channel();
		std::string getName();
		std::vector<Client *> getListClients();
		void addClient(Client *client);

	private:
		Channel();
		Channel(Channel const &src);
		Channel &operator=(Channel const &src);
		std::string _name;
		std::vector<Client *> _list_clients;
};

#endif
