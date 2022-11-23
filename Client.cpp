#include "Client.hpp"
#include "Server.hpp"

Client::Client(int fd, struct sockaddr_in address):
	_fd(fd), _hostname()
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
	char hostname[NI_MAXHOST];

	//getnameinfo(struct sockaddr *addr, socklen_t salen, char *host, size_t hostlen, char *serv, size_t servlen, int flags)
	//NI_MAXHOST = 1025 et NI_MAXSERV = 32 (taille des buffers), c'est <netdb.h> qui definit ces constantes
	//NI_NUMERICSERV ---> si cet attribut est défini, l'adresse du service est renvoyée sous forme numérique
	if (getnameinfo((struct sockaddr *)&address, sizeof(address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
		throw Client::ErrnoEx(); //surement mettre une autre exception
	else
		this->_hostname = hostname;
}


void Client::receive()
{

	char buffer[4097]; //à verif si on met ça ou pas
	ssize_t size;
	if ((size = recv(_fd, &buffer, 4096, 0)) == -1)
		return;

	if (size == 0)
	{
		_status = DELETE;
		return;
	}
	buffer[size] = 0;

	this->_buffer += buffer;

	//pas fini
}


Client::~Client()
{
	close(_fd);
}

void Client::setStatus(UserStatus status)
{
	this->_status = status;	
}
