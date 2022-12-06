#include "Client.hpp"
#include "Server.hpp"

Client::Client(int sock, struct sockaddr_in address):
_sock(sock), _msg_finish(0), _status(TO_REGISTER), _hostname(),_nickname(), _username(), _realname(), _oper(false), _client_channels()
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
	char buffer[MAX_CHAR + 1]; //VALIDER LA TAILLE DE MAX_CHAR ???
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
	// If recv return 0 that means the pair stop the connection normally
	if (size == 0)
	{
		_msg.clear();
		_status = REMOVE_ME;
		return;
	}
	buffer[size] = 0;
	_msg += buffer;
	//check if msg is end with \r\n
	if (_msg.size() > 2 && *(_msg.end() - 2) ==  '\r' && *(_msg.end() - 1) == '\n')
	{
		_msg_finish = 1;
		splitCommand();
	}
	else
		_msg_finish = 0;
	if (DEBUG)
		std::cout << "Client SOCK: |" << _sock << "| MSG BRUT |" << _msg  << "|" << std::endl;
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

int Client::getSock()
{
	return (_sock);
}

userStatus Client::getStatus()
{
	return (_status);
}

void Client::setStatus(userStatus status)
{
	_status = status;	
}

std::vector<std::vector<std::string> > *Client::getCmd()
{
	return (&_cmd);
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

std::string Client::getRealname()
{
	return (_realname);
}

std::string Client::getHostname()
{
	return (_hostname);
}

void Client::setNickname(std::string nickname)
{
	_nickname = nickname;
}

void Client::setUsername(std::string username)
{
	_username = username;
}

//dans USER (on suppr les ":" au debut)
void Client::setRealname(std::string realname)
{
	realname.erase(realname.begin());
	_realname = realname;
}

bool Client::getOper()
{
	return (_oper);
}

void Client::setOper(bool oper)
{
	_oper = oper;
}

std::string Client::getOptions()
{
	std::string msg;

	msg = "+ ";
	msg += _options;
	return (msg);
}

bool Client::IsOptionSet(std::string check, std::string option)
{
	if (check.find(option) != std::string::npos)
			return (true);
	return (false);
}

void Client::changeOptions(std::string options, bool add)
{
	if (add)
	{
		if (!IsOptionSet(_options, options))
			_options += options;
	}
	else
	{
		if (IsOptionSet(_options, options))
			_options.erase(_options.find(options));
	}

}

void Client::addChannel(Channel *channel)
{
	_client_channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
	for (std::vector<Channel *>::iterator it = _client_channels.begin() ; it != _client_channels.end() ; ++it)
	{
		if ((*it) == channel)
		{
			_client_channels.erase(it);
			return ;
		}
	}
}
std::vector<Channel *> *Client::getClientChannels()
{
	return (&_client_channels);
}
