#include "Server.hpp"

Server::Server(std::string port, std::string password):
_port(port), _pass(password)
{

	int sock;
	// int socket(int domain, int type, int protocol);
	// domain: PF_INET Protocoles Internet IPv4 ou PF_INET6 Protocoles Internet IPv6
	// type: SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams. An out-of-band data transmission mechanism may be supported.
	if ((sock = socket(PF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto)) == 0)
		throw Server::ExErrno();

	// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
	// manipulate options for the socket referred to by the file descriptor sockfd
	// SOL_SOCKET is the socket layer itself. It is used for options that are protocol independent.
	// SO_REUSEADDR This option controls whether bind should permit reuse of local addresses for this socket. If you enable this option, you can actually have two sockets with the same Internet port number; but the system won’t allow you to use the two identically-named sockets in a way that would confuse the Internet. The reason for this option is that some higher-level Internet protocols, including FTP, require you to keep reusing the same port number.
	int enable = 1; // CHECK ENABLE
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)))
		throw Server::ExErrno();

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
	/*if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
	throw Server::ExErrno();
	}*/
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

	// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	/*
	   When a socket is created with socket(2), it exists in a name
	   space (address family) but has no address assigned to it.  bind()
	   assigns the address specified by addr to the socket referred to
	   by the file descriptor sockfd.  addrlen specifies the size, in
	   bytes, of the address structure pointed to by addr.
	   Traditionally, this operation is called “assigning a name to a
	   socket”.
	*/
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw Server::ExErrno();

	if (listen(sock, addr.sin_port) < 0)
			throw Server::ExErrno();
}

Server::~Server()
{

}
/*
std::string const &Server::getPort() const
{
	return (this->port);
}

std::string const &Server::getPassword() const
{
	return (this->password);
}

int const &Server::getSocket() const
{
	return (this->socket);
}
*/
