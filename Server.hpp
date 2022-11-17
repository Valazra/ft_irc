#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>

class Server
{
	public:
		Server(std::string port, std::string password);
		virtual ~Server();

		std::string const &getPort() const;
		std::string const &getPassword() const;

	private:
		Server();
		Server(Server const &src);
		Server &operator=(Server const &src);

		std::string _port;
		std::string _password;
};

#endif
