#include "Server.hpp"

Server::Server(std::string port, std::string password): _port(port), _pass(password)
{

	int listened_sock;
	// int sock(int domain, int type, int protocol);
	// domain: PF_INET Protocoles Internet IPv4 ou PF_INET6 Protocoles Internet IPv6
	// type: SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
	if ((listened_sock = socket(PF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto)) == 0)
		throw Server::StderrException();

	// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
	// manipulate options for the socket referred to by the file descriptor sockfd
	// SOL_SOCKET is the socket layer itself. It is used for options that are protocol independent.
	// SO_REUSEADDR This option controls whether bind should permit reuse of local addresses for this socket. If you enable this option, you can actually have two sockets with the same Internet port number; but the system won’t allow you to use the two identically-named sockets in a way that would confuse the Internet. The reason for this option is that some higher-level Internet protocols, including FTP, require you to keep reusing the same port number.
	/*
	 *Sometimes, you might notice, you try to rerun a server and bind() fails, claiming “Address already in use.” What does that mean? Well, a little bit of a socket that was connected is still hanging around in the kernel, and it’s hogging the port. You can either wait for it to clear (a minute or so), or add code to your program allowing it to reuse the port, like this:
	 */
	int enable = 1; // CHECK ENABLE
	if (setsockopt(listened_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)))
		throw Server::StderrException();

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
		throw Server::StderrException();

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
		throw Server::StderrException();

	//10 est le nombre max de clients à faire la queue
	if (listen(listened_sock, 10) < 0)
		throw Server::StderrException();

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
	std::vector<Client *> clients = getClients();


/*
	// CA NE COMPILE PLUS C NORMAL IL Y A PLEIN DE FONCTIONS QUI MANQUE ICI
	// METTRE SERVER RUN EN COMMENTAIRE PR COMPILER
	//RAPPEL NOTRE _fds[0] C EST NOTRE LISTENED_SOCK

//on a pas encore créé nos users, il faut faire une classe User et tout et c'est long sa mere
	std::vector<User *> users = getUsers();

	// il fait quoi exactement avec son atoi de ping? config get j ai pas tout pige
	int ping = atoi(config.get("ping").c_str());

	
	 // int poll(struct pollfd fds[], nfds_t nfds, int timeout);
	 //fds is our array of information (which sockets to monitor for what),
	 //nfds is the count of elements in the array, and timeout is a timeout in milliseconds.
	 //It returns the number of elements in the array that have had an event occur.
	 
	if (poll(&_fds[0], _fds.size(), (ping * 1000) / 10) == -1)
		return;

	if (std::time(0) - last_ping >= ping)
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
	   //qu'on a deja accepte donc on creer une fonction receive (ou autre nom) ou on va stocke
	   //et traite les infos recus
		if (_fds[0].revents == POLLIN)
			acceptUser();
		else
			for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
				if ((*it).revents == POLLIN)
					this->users[(*it).fd]->receive(this);
	}
	//on check si on doit pas supprimer des utilisateurs
	for (std::vector<irc::User *>::iterator it = users.begin(); it != users.end(); ++it)
		if ((*it)->getStatus() == DELETE)
			delUser(*(*it));
	//la je sais ap ce qu'il fait
	users = getUsers();
	for (std::vector<irc::User *>::iterator it = users.begin(); it != users.end(); ++it)
		(*it)->push();
	displayUsers();
*/
}

//recupere la liste des clients dans un vector
std::vector<Client *> Server::getClients()
{
	std::vector<Client *> clients = std::vector<Client *>();
	for (std::map<int, Client *>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
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
