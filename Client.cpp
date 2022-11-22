#include "Client.hpp"

Client::Client(int fd, struct sockaddr_in address) : _fd(fd), _hostname()
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
	char hostname[NI_MAXHOST];

//getnameinfo(struct sockaddr *addr, socklen_t salen, char *host, size_t hostlen, char *serv, size_t servlen, int flags)
//NI_MAXHOST = 1025 et NI_MAXSERV = 32 (taille des buffers), c'est <netdb.h> qui definit ces constantes
//NI_NUMERICSERV ---> si cet attribut est défini, l'adresse du service est renvoyée sous forme numérique
	if (getnameinfo((struct sockaddr *)&address, sizeof(address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
		throw Client::StderrException(); //surement mettre une autre exception
	else
		this->_hostname = hostname;
}

Client::~Client()
{
	close(_fd);
}
