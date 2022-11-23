#include "Server.hpp"

Server::Server(std::string port, std::string password): _port(port), _pass(password)
{

	int listened_sock;
	// int sock(int domain, int type, int protocol);
	// domain: PF_INET Protocoles Internet IPv4 ou PF_INET6 Protocoles Internet IPv6
	// type: SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
	if ((listened_sock = socket(PF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto)) == 0)
		throw Server::ErrnoEx();

	// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
	// manipulate options for the socket referred to by the file descriptor sockfd
	// SOL_SOCKET is the socket layer itself. It is used for options that are protocol independent.
	// SO_REUSEADDR This option controls whether bind should permit reuse of local addresses for this socket. If you enable this option, you can actually have two sockets with the same Internet port number; but the system won’t allow you to use the two identically-named sockets in a way that would confuse the Internet. The reason for this option is that some higher-level Internet protocols, including FTP, require you to keep reusing the same port number.
	/*
	 *Sometimes, you might notice, you try to rerun a server and bind() fails, claiming “Address already in use.” What does that mean? Well, a little bit of a socket that was connected is still hanging around in the kernel, and it’s hogging the port. You can either wait for it to clear (a minute or so), or add code to your program allowing it to reuse the port, like this:
	 */
	int enable = 1; // CHECK ENABLE
	if (setsockopt(listened_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)))
		throw Server::ErrnoEx();

	/*
	 * int fcntl(int fd, int cmd, ... * arg *);
	 * fcntl() performs one of the operations described below on the open file
	 * descriptor fd.  The operation is determined by cmd. 
	 * F_SETFL  Set the file status flags to the value specified by arg
	 * Lots of functions block. accept() blocks. All the recv() functions block. The reason they can do this is because they're allowed to. When you first create the socket descriptor with socket(), the kernel sets it to blocking. If you don't want a socket to be blocking, you have to make a call to fcntl():
	 * LIEN DU DESSOUS QUI DIT PLUTOT UTILISER SELECT QUE UTILISER NONBLOCK A VOIR SI SA FIT AVEC LE SUJET ON LAISSE COMME CA EN ATT
	 *https://www.gta.ufrj.br/ensino/eel878/sockets/advanced.html#blocking 
	*/
	if (fcntl(listened_sock, F_SETFL, O_NONBLOCK) < 0)
		throw Server::ErrnoEx();

	// (IPv4 only--see struct sockaddr_in6 for IPv6)
	/*
	   To deal with struct sockaddr, programmers created a parallel structure: struct sockaddr_in (“in” for “Internet”) to be used with IPv4.
	   And this is the important bit: a pointer to a struct sockaddr_in can be cast to a pointer to a struct sockaddr and vice-versa.
	   struct sockaddr_in {
	   	short int          sin_family;  // Address family, AF_INET
	   	unsigned short int sin_port;    // Port number
	   	struct in_addr     sin_addr;    // Internet address
	   	unsigned char      sin_zero[8]; // Same size as struct sockaddr
	   };
	*/
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; //ipv4

	/* When INADDR_ANY is
	   specified in the bind call, the socket will be bound to all local
	   interfaces.  When listen(2) is called on an unbound socket, the
	   socket is automatically bound to a random free port with the
	   local address set to INADDR_ANY.
	   */
	addr.sin_addr.s_addr = INADDR_ANY;

	// The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
	addr.sin_port = htons(atoi(_port.c_str()));

	/*
	   When a socket is created with socket(2), it exists in a name
	   space (address family) but has no address assigned to it.  bind()
	   assigns the address specified by addr to the socket referred to
	   by the file descriptor sockfd.  addrlen specifies the size, in
	   bytes, of the address structure pointed to by addr.
	   Traditionally, this operation is called “assigning a name to a
	   socket”.
	*/
	if (bind(listened_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw Server::ErrnoEx();

	//10 est le nombre max de clients à faire la queue
	if (listen(listened_sock, 10) < 0)
		throw Server::ErrnoEx();

	std::cout << "IRC server has been initialized" << std::endl;
	std::cout << "Waiting for clients" << std::endl;

	//on ajoute 1 elem pollfd sur notre vector (tableau) de pollfd
	//on remplit notre struct avec notre sock et POLLIN pour dire qu'on veut
	//etre avertit si on recoit des donnees sur cette socket
	_fds.push_back(pollfd());
	_fds.back().fd = listened_sock;
	_fds.back().events = POLLIN;

	this->set("user_mode", "aiwro");
//des set à rajouter
}

Server::~Server()
{
}

void Server::run()
{
	//RAPPEL NOTRE _fds[0] C EST NOTRE LISTENED_SOCK

	// ces zinzins remplissent leur vector de client avec getclient qui
	// trouve ces clients dans un map 
	std::vector<Client *> clients = getClients();

	 // int poll(struct pollfd fds[], nfds_t nfds, int timeout);
	 //fds is our array of information (which sockets to monitor for what),
	 //nfds is the count of elements in the array, and timeout is a timeout in milliseconds.
	 //It returns the number of elements in the array that have had an event occur.
	// de ce que je comprends si jamais notre poll sur notre fd listen se branle trop
	// on se casse de server::run on retourne dans leur main qui relance la boucle
	// donc on reviendra ici jusqu'a ce qu' on est quelque chose
	// mtn le probleme de leur truc c est que si il y a un autre type de bug ca 
	// renvoie aussi -1 et on gere pas l erreur
	if (poll(&_fds[0], _fds.size(), (PING * 1000) / 10) == -1)
		return;

	//irc deco les clients qui repondent pas a leur ping a temps ca doit 
	//etre un truc en rapport
	if (std::time(0) - last_ping >= PING)
	{
		sendPing();
		last_ping = std::time(0);
	}
	else
	{
	   //en gros on regarde dans les events POLLIN qu'on a recu, si c'est la 
	   //socket 0 c est un nouvel utilisateur donc on appel un truc pour voir
	   //si on va accept cet utilisateur tcheck mot de passe etc...
	   //ELSE si c'est un autre socket qui POLLIN c est que c est un gars
	   //qu'on a deja accepte donc on creer une fonction recieve (ou autre nom) ou on va stocke
	   //et traite les infos recus
		if (_fds[0].revents == POLLIN)
			check_new_client();
		else
			for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
			{
				if ((*it).revents == POLLIN)
				{
					_clients[(*it).fd]->receive();
					if (_clients[(*it).fd]->getStatus() == REMOVE_ME)
						removeClient((*it).fd);
					else if (_clients[(*it).fd]->getMsgFinish()) //le _msg_finish change dans client.receive()
					{
						treat_complete_msg((*it).fd);
					}
				}
			}
	}
}

void Server::check_new_client()
{
	//int shutdown(int sockfd, int how);
	// The shutdown() call causes all or part of a full-duplex
    //  connection on the socket associated with sockfd to be shut down.
	//If how is SHUT_RD, further receptions will be disallowed
	//donc en gros on dit qu'on prendra plus de nouveaux clients a notre
	//listened socket
	if (_clients.size() == MAX_CLIENTS) 
		if (shutdown(_fds[0].fd, SHUT_RD) == -1)
			throw Server::ErrnoEx();
	struct sockaddr_in addr;
	// j ai pas compris la ligne d apres
	socklen_t sock_len = sizeof(addr);
	int new_client_sock = accept(_fds[0].fd, (struct sockaddr *)&addr, &sock_len);
	if (new_client_sock == -1)
		throw Server::ErrnoEx();
	// et donc la ils creent le nouveau client qu'ils rajoutent a la map de clients
	_clients[new_client_sock] =   new Client(new_client_sock, addr);
	// Si y'a pas de mdp on enregistre le client 
	if (!(_pass.length())) 
		_clients[new_client_sock]->setStatus(REGISTER);
	//on l'ajoute a notre vector de struct pollfd en ecoute
	_fds.push_back(pollfd());
	_fds.back().fd = new_client_sock;
	_fds.back().events = POLLIN;
}

void	Server::treat_complete_msg(int const &client_sock)
{
	// on regarde si on a plusieurs command dans notre _msg, on les split
	// dans un vector de string du coup mtn on utilise _cmd
	_clients[client_sock]->splitCommand();
	if (_clients[client_sock]->getStatus() == TO_REGISTER)
		registerClient(client_sock);
//	else
//		_command_book.find_command(client->getCommand().front(), client, _all_clients, &_all_channels);
//	_client[client_sock]->clearMessage();
//	_clients[client_sock]->clearCommand();
//	_clients[client_sock]->clearCommand();
//	this->find_to_kill();

}

void	Server::registerClient(int const &client_sock)
{
(void)client_sock;
/*	std::vector<std::string> full_command(_client[client_sock]->getCommand());
	std::vector<std::string> tmp = full_command;
	while (full_command.empty() == false)
	{

		_command_book.find_command(full_command.front(), client, _all_clients, &_all_channels);
		full_command.erase(full_command.begin());
	}
	if (client->getRegNick() == true && client->getRegUser() == true)
	{
		std::cout << GREEN << "********REGISTRATION SUCCESS for " << client->getNickname() << "**********" << RESET << std::endl;
		client->setRegistration(true);
		ft_reply("001", client, NULL, "");
		ft_reply("002", client, NULL, "");
		ft_reply("003", client, NULL, "");
		ft_reply("004", client, NULL, "");
		ft_reply(RPL_CUSTOMMOTD, client, NULL, _pokemon);
		if (client->getRegPass() == true)
		{
			if (client->getPassword() != this->_password)
			{
				ft_error(ERR_PASSWDMISMATCH, client, NULL, "");
				client->setRegPass(false);
			}
		}
	}*/
}

void	Server::removeClient(int const &sock_to_remove)
{
	close(sock_to_remove);
//faut iterer sur le map ou vector client pour erase je pense
//	_clients[sock_to_remove].erase();
}

//à faire
void Server::sendPing()
{
}

//recupere la liste des clients dans un vector
std::vector<Client *> Server::getClients()
{
	std::vector<Client *> clients = std::vector<Client *>();
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
		clients.push_back(it->second);
	return (clients);
}

void Server::set(std::string key, std::string value)
{
	_values[key] = value;
}

std::string Server::get(std::string key)
{
	return (_values[key]);
}
