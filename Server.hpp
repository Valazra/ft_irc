#ifndef SERVER_HPP
# define SERVER_HPP

class Server
{
	public:
		Server(std::string port, std::string password);
		virtual ~Server();
	private:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);
};

#endif
