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
		std::vector<Client *> *getListClients();
		Client *getChannelOperator();
		std::string getTopic();
		void setTopic(std::string topic);
		bool getHasTopic();
		void setHasTopicOn();
		void setHasTopicOff();
		std::string getCreationTimeTopic();
		void deleteTopic();
		void addClient(Client *client);
		void deleteClient(Client *client);
		std::string getTime();
		int howManyClients();
		bool isChanOp();
		void setChanOp(bool set);

	private:
		Channel();
		Channel(Channel const &src);
		Channel &operator=(Channel const &src);
		std::string _name;
		std::vector<Client *> _list_clients;
		Client *_chan_operator;
		std::string _topic;
		bool _has_topic;
		bool _is_chan_op;
		std::string _creation_time_topic;
};

#endif
