#include "Server.hpp"

Server::Server(std::string port, std::string password): 
_port(port), _pass(password), _fatal_error(false) ,_cmd(&_clients, password, &_fatal_error), _sock_to_remove()
{
	int listened_sock;
	// int socket(int domain, int type, int protocol);
	// domain: PF_INET Protocoles Internet IPv4 ou PF_INET6 Protocoles Internet IPv6
	// type: SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
	if ((listened_sock = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, getprotobyname("tcp")->p_proto)) == 0)
	{
		quit = true;
		return ;
	}
	// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
	// manipulate options for the socket referred to by the file descriptor sockfd
	// SOL_SOCKET is the socket layer itself. It is used for options that are protocol independent.
	// SO_REUSEADDR This option controls whether bind should permit reuse of local addresses for this socket. If you enable this option, you can actually have two sockets with the same Internet port number; but the system won’t allow you to use the two identically-named sockets in a way that would confuse the Internet. The reason for this option is that some higher-level Internet protocols, including FTP, require you to keep reusing the same port number.
	int enable = 1; 
	if (setsockopt(listened_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)))
	{
		quit = true;
		close(listened_sock);
		return ;
	}
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
	{
		close(listened_sock);
		quit = true;
		return ;
	}
	if (listen(listened_sock, MAX_CLIENTS) < 0)
	{
		close(listened_sock);
		quit = true;
		return ;
	}
	_fds.push_back(pollfd());
	_fds.back().fd = listened_sock;
	_fds.back().events = POLLIN;
	std::cout << "Welcome." << std::endl;
	std::cout << "Our IRC server has been initialized." << std::endl;
	std::cout << "Waiting for clients..." << std::endl;
}

Server::~Server()
{
	if (DEBUG)
	std::cout << "Server::Destructor" << std::endl;
	std::vector<Client *> tmp;
	for(std::map<int, Client *>::iterator it = _clients.begin() ; it != _clients.end() ; ++it)
		tmp.push_back((*it).second);
	for (std::vector<Client *>::iterator it = tmp.begin() ; it != tmp.end() ; ++it)
			delete (*it);
}

bool Server::isSocketStillOpen(int sock)
{
	for (std::vector<int>::iterator it = _sock_to_remove.begin(); it != _sock_to_remove.end(); ++it)
	{
		if (sock == *it)
			return (0);
	}
	return (1);
}

void Server::run()
{
	 // int poll(struct pollfd fds[], nfds_t nfds, int timeout);
	 //fds is our array of information (which sockets to monitor for what),
	 //nfds is the count of elements in the array, and timeout is a timeout in milliseconds.
	 //It returns the number of elements in the array that have had an event occur.
	if (poll(&_fds[0], _fds.size(), -1) == -1)
	{
		if (DEBUG)
			std::cout << "Server::run poll()" << std::endl;
		quit = true;
		return;
	}
	if (_fds[0].revents == POLLIN)
		check_new_client();
	else
	{
		for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
		{
			if ((*it).revents == POLLIN)
			{
				if (!isSocketStillOpen((*it).fd))
				{
					continue ;
				}
				_clients[(*it).fd]->receive();
				if (_clients[(*it).fd]->getStatus() == REMOVE_ME)
				{
					removeClient((*it).fd);
					_sock_to_remove.push_back((*it).fd);
				}
				else if (_clients[(*it).fd]->getMsgFinish())
				{
					_fatal_error = false;
					_cmd.readCmd((*it).fd);
					if (!((_cmd.getSockToRemove())->empty()))
					{
						for (std::vector<int>::iterator it1 = (_cmd.getSockToRemove())->begin(); it1 != (_cmd.getSockToRemove())->end(); ++it1)
						{
							_sock_to_remove.push_back(*it1);
							removeClient(*it1);
						}
					}
				}
			}
		}
		for (std::vector<int>::iterator it = _sock_to_remove.begin(); it != _sock_to_remove.end(); ++it)
		{
			for (std::vector<pollfd>::iterator it1 = _fds.begin(); it1 != _fds.end(); ++it1)
			{
				if (*it == (*it1).fd)
				{
					_fds.erase(it1);
					it1 = _fds.end() - 1;
				}
			}
		}
		_sock_to_remove.clear();
	}
}

void Server::check_new_client()
{
	//int shutdown(int sockfd, int how);
	//The shutdown() call causes all or part of a full-duplex
    	//connection on the socket associated with sockfd to be shut down.
	//If how is SHUT_RD, further receptions will be disallowed
	if (_clients.size() == MAX_CLIENTS) 
	{
		if (shutdown(_fds[0].fd, SHUT_RD) == -1)
		{
			quit = true;
			return ;	
		}
	}
	struct sockaddr_in addr;
	socklen_t sock_len = sizeof(addr);
	int new_client_sock = accept(_fds[0].fd, (struct sockaddr *)&addr, &sock_len);
	if (new_client_sock == -1)
	{
		quit = true;
		return ;	
	}
	if (DEBUG)
		std::cout << new_client_sock << std::endl;
	_clients[new_client_sock] = new Client(new_client_sock);
	_fds.push_back(pollfd());
	_fds.back().fd = new_client_sock;
	_fds.back().events = POLLIN;
}

void	Server::removeClient(int const sock_to_remove)
{	
	std::vector<Channel *>* listChannels = ((_clients)[sock_to_remove])->getClientChannels();
	for(std::vector<Channel *>::iterator it = listChannels->begin() ; it != listChannels->end() ; ++it)
	{
		if ((*it)->isChanOp())
			if ((*it)->getChannelOperator() == (_clients)[sock_to_remove])
				(*it)->setChanOp(false);
		(*it)->deleteClient((_clients)[sock_to_remove]);
	}
	((_clients)[sock_to_remove])->leaveAllChannels();
	_cmd.checkIfEmptyChan();
	close(sock_to_remove);
	delete (_clients)[sock_to_remove];
	_clients.erase(sock_to_remove);
}

std::vector<pollfd>* Server::getFds()
{
	return (&_fds);
}
