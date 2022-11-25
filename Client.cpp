#include "Client.hpp"
#include "Server.hpp"

Client::Client(int sock, struct sockaddr_in address):
_sock(sock), _hostname(), _msg_finish(0), _status(TO_REGISTER), _nickname(), _username()
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
	{
		_msg_finish = 1;
		splitCommand();
	}
	else
		_msg_finish = 0;
	std::cout << _msg << std::endl << std::endl << std::endl;
}

void Client::splitCommand()
{
	int i = 0;
	int new_start = 0;
	int number_cmd = 0;
	while (_msg[i] && _msg[i+1])
	{
		if (_msg[i] ==  '\r' && _msg[i + 1] == '\n')
		{
			std::string tmp_cmd(_msg.substr(new_start, i - new_start));
			new_start = i + 2;
			i += 2;

			int j = 0;
			int split_start = 0;
			int special_case = 0;
			while (tmp_cmd[j])
			{
				if (tmp_cmd[j] == ' ' || !tmp_cmd[j + 1])
				{
					if (!tmp_cmd[j + 1] && tmp_cmd[j] != ' ')
						special_case = 1;
					std::string split_split(tmp_cmd.substr(split_start, (j + special_case) - split_start));
					_cmd.push_back(std::vector<std::string>());
					_cmd[number_cmd].push_back(split_split);
					split_start = j + 1;
				}
				j++;
			}
			number_cmd++;

		}
		else
			i++;
	}
	for (std::vector<std::vector<std::string> >::iterator it = _cmd.begin(); it != _cmd.end(); ++it)
	{
		for (std::vector<std::string>::iterator it2 = it->begin(); it2 != it->end(); ++it2)
			std::cout << "|" << *it2 << "|" << std::endl;
	}
}

Client::~Client()
{
	close(_sock);
}


userStatus Client::getStatus()
{
	return (_status);
}

void Client::setStatus(userStatus status)
{
	_status = status;	
}

std::vector<std::vector<std::string> > Client::getCmd()
{
	return (_cmd);
}

bool Client::getMsgFinish()
{
	return (_msg_finish);
}

std::string Client::getNickname()
{
	return (_nickname);
}

std::string Client::getUsername()
{
	return (_username);
}

void Client::setNickname(std::string nickname)
{
	_nickname = nickname;
}

void Client::setUsername(std::string username)
{
	_username = username;
}
