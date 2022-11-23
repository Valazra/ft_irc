#include "Client.hpp"
#include "Server.hpp"

Client::Client(int sock, struct sockaddr_in address):
_sock(sock), _hostname(), _msg_finish(0), _status(TO_REGISTER)
{
	fcntl(sock, F_SETFL, O_NONBLOCK);
	char hostname[NI_MAXHOST];

	//getnameinfo(struct sockaddr *addr, socklen_t salen, char *host, size_t hostlen, char *serv, size_t servlen, int flags)
	//NI_MAXHOST = 1025 et NI_MAXSERV = 32 (taille des buffers), c'est <netdb.h> qui definit ces constantes
	//NI_NUMERICSERV ---> si cet attribut est défini, l'adresse du service est renvoyée sous forme numérique
	if (getnameinfo((struct sockaddr *)&address, sizeof(address), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
		throw Client::ErrnoEx(); //surement mettre une autre exception
	else
		this->_hostname = hostname;
}

void Client::splitCommand()
{
	int i = 0;
	int new_start = 0;
	while (_msg[i] && _msg[i+1])
	{
		if (_msg[i] ==  '\r' && _msg[i + 1] == '\n')
		{
			_cmd.push_back(_msg.substr(new_start, i - new_start));
			new_start = i + 2;
			i += 2;
		}
		else
			i++;
	}
	for (std::vector<std::string>::iterator it = _cmd.begin(); it != _cmd.end(); ++it)
	std::cout << *it << "|" << std::endl;

}

void Client::receive()
{

	char buffer[MAX_CHAR + 1]; //à verif si on met ça ou pas
	memset(buffer, 0, MAX_CHAR);
	if (_msg_finish)
	{
		_msg.clear();
		_msg_finish = 0;
	}
	//exception error plutot que return?
	ssize_t size;
	if ((size = recv(_sock, &buffer, MAX_CHAR, 0)) == -1)
	{
		_msg.clear();
		return;
	}
	if (size == 0)
	{
		_msg.clear();
		_status = REMOVE_ME;
		return;
	}
	buffer[size] = 0;

	_msg += buffer;
	//check if msg if end with \r follow by \n
	if (_msg.size() > 2 && *(_msg.end() - 2) ==  '\r' && *(_msg.end() - 1) == '\n')
		_msg_finish = 1;
	else
		_msg_finish = 0;
	std::cout << _msg << std::endl << std::endl << std::endl;

	//pas fini
}


Client::~Client()
{
	close(_sock);
}


userStatus Client::getStatus()
{
	return (_status);
}

bool Client::getMsgFinish()
{
	return (_msg_finish);
}

void Client::setStatus(userStatus status)
{
	_status = status;	
}
