#include "Client.hpp"

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

/*
void irc::User::receive(Server *server)
{
	{
		char buffer[BUFFER_SIZE + 1];
		ssize_t size;
		if ((size = recv(fd, &buffer, BUFFER_SIZE, 0)) == -1)
			return;

		if (size == 0)
		{
			status = DELETE;
			return;
		}
		buffer[size] = 0;

		this->buffer += buffer;
	}

	std::string delimiter(MESSAGE_END);
	size_t position;
	while ((position = buffer.find(delimiter)) != std::string::npos)
	{
		std::string message = buffer.substr(0, position);
		buffer.erase(0, position + delimiter.length());
		if (!message.length())
			continue;

		if (DEBUG)
		{
			time_t t = time(0);
			struct tm * now = localtime( & t );
			char buffer[80];
			strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", now);
			std::cout << "[" << buffer << "] " << fd << " < " << message << std::endl;
		}
		commands.push_back(new Command(this, server, message));
	}
	dispatch();
}
*/

Client::~Client()
{
	close(_fd);
}
