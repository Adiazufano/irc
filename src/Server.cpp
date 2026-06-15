#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "commands.hpp"

extern bool run_server;

// Default constructor
Server::Server() {}

// Destructor
Server::~Server()
{
	std::cout << "\nClosing server...\n";
	freeaddrinfo(_addr_lst);
	close(_serv_socket);
	std::map<std::string, Channel*>::iterator it;
	for(it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	_channels.clear();
	std::cout << "Bye!" << std::endl;
}

// Parameterized constructor
Server::Server(char *port, char* password) : _port(port), _password(password), _serv_socket(-1), _addr_lst(NULL)
{}

// Copy constructor (shouldn't be called)
Server::Server(const Server &other)
{
	(void)other;
}

// Copy operator constructor (shouldn't be called)
Server &Server::operator=(const Server& other)
{
	(void)other;
	return (*this);
}

void sigint_handler(int signal)
{
	(void)signal;
	run_server = false;
}

std::map<std::string, Channel *> &Server::getChannels()
{
	return _channels;
}

std::map<std::string, int> &Server::getClientsByNick()
{
	return _clientsByNick;
}

std::map<int, Client> &Server::getClients()
{
	return _clients;
}

const std::string &Server::getHostname() const
{
    return _hostname;
}

void Server::init()
{
	// Network address
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;		// localhost
	int opt = 1;

	int gai = getaddrinfo(0, _port.data(), &hints, &_addr_lst);
	if (gai != 0)
		throw std::runtime_error("\033[1;31mgetaddrinfo error:\033[0m " + std::string(gai_strerror(gai)));

	// Server socket
	_serv_socket = socket(_addr_lst->ai_family, _addr_lst->ai_socktype, _addr_lst->ai_protocol);
	if (_serv_socket < 0)
		throw std::runtime_error("\033[1;31msocket error:\033[0m " + std::string(strerror(errno)));
	if (setsockopt(_serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("\033[1;31msetsockopt error:\033[0m " + std::string(strerror(errno)));
	if (bind(_serv_socket, _addr_lst->ai_addr, _addr_lst->ai_addrlen) < 0)
		throw std::runtime_error("\033[1;31mbind error:\033[0m " + std::string(strerror(errno)));
	
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) < 0)
		throw std::runtime_error("\033[1;31mgethostname error:\033[0m " + std::string(strerror(errno)));
	_hostname = hostname;

	struct pollfd serv_pfd = { _serv_socket, POLLIN, 0 };
	_pfd_arr.push_back(serv_pfd);

	// Listen
	if (listen(_serv_socket, SOMAXCONN) < 0)
		throw std::runtime_error("\033[1;31mlisten error:\033[0m " + std::string(strerror(errno)));

	std::cout << "Server listening on port " << _port.c_str() << '\n';
}

void Server::accept_socket()
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int fd = accept(_serv_socket, (struct sockaddr*)&client_addr, &client_len);
	if (fd > 0)
	{
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
		_accepted_sockets.push_back(fd);
		_accepted_ips[fd] = std::string(ip);
		std::cout << "New connection accepted: Socket " << fd << " Client IP: " << ip << '\n';
	}
} 

void Server::client_event(int fd)
{
	char buffer[512] = { 0 };
	ssize_t n_bytes = recv(fd, buffer, sizeof(buffer), 0);
	if (n_bytes < 0)
	{
		std::cout << "recv error: " << strerror(errno) << std::endl;
		_disconnected_sockets.push_back(fd);
	}
	else if (n_bytes == 0)
	{
		std::cout << "Client on socket " << fd << " disconnected\n";
		_disconnected_sockets.push_back(fd);
	}
	else if (n_bytes > 0)
	{
		//buffer acumulativo para cada cliente
		Client &cli = _clients[fd];
		cli.buffer += std::string(buffer, n_bytes);
		std::string &buf = cli.buffer;
		size_t pos;
		//no considerar mensaje completo hasta encontrar "\r\n"
		while ((pos = buf.find("\r\n")) != std::string::npos)
		{
			std::string message = buf.substr(0, pos);
			buf.erase(0, pos + 2);
			std::cout << "Message from socket " << cli.getFd() << ": " << message << "\n";
			//parseo de comandos de autentificacion
			if (!cli.getRegistered())
				commandParse(message, cli, _password, *this);
			// To Do: No hay que dejar validar comandos hasta que no hayamos confirmado correctamente la conexión del usuario.
			else
				validate_command(*this, message, cli);
		}

		//si autentificacion mandar mensajes
		if (cli.getHasPass() && !cli.getNickname().empty() && !cli.getUser().empty() && cli.getAuthenticated() && !cli.getRegistered())
		{
			//cli.setAuthenticated(true);
			std::string nick = cli.getNickname();
			cli.sendMsg(RPL_WELCOME(nick));
			cli.sendMsg(RPL_YOURHOST(nick));
			cli.sendMsg(RPL_CREATED(nick));
			cli.sendMsg(RPL_MYINFO(nick));
			cli.sendMsg(RPL_ISUPPORT(nick));
			cli.sendMsg(RPL_ENDOFMOTD(nick));

			cli.setRegistered(true);
		}
	}
}

void Server::add_clients()
{
	std::cout << "Adding " << _accepted_sockets.size() << " new clients" << '\n';
	for (size_t i = 0; i < _accepted_sockets.size(); i++)
	{
		int fd = _accepted_sockets[i];
		struct pollfd pfd = { fd, POLLIN, 0 };
		_pfd_arr.push_back(pfd);
		_clients.insert(std::make_pair(fd, Client(fd, _accepted_ips[fd])));
		_accepted_ips.erase(_accepted_sockets[i]);
	}
	_accepted_sockets.clear();
}

std::vector<int>	&Server::getDisconnectedSockets()
{
	return (_disconnected_sockets);
}

void Server::disconnect_sockets()
{
	std::cout << "Removing " << _disconnected_sockets.size() << " disconnected clients\n";
	for (size_t i = 0; i < _disconnected_sockets.size(); i++)
	{
		std::vector<struct pollfd>::iterator it = _pfd_arr.begin();
		while (it != _pfd_arr.end())
		{
			if (_disconnected_sockets[i] == it->fd)
			{
				close(it->fd);
				_clientsByNick.erase(_clients[it->fd].getNickname());
				_clients.erase(it -> fd);
				it = _pfd_arr.erase(it);
				break;
			}
			it++;
		}
	}
	_disconnected_sockets.clear();

	// Remove empty channels
	std::map<std::string, Channel*>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); )
	{
		if (it->second->getClientsArray().size() == 0)
		{
			std::cout << "Removing empty channel: " << it->first << '\n';
			delete it->second;
			_channels.erase(it++);
		}
		else
			++it;
	}
}

void Server::run()
{
	signal(SIGINT, sigint_handler);

	while (run_server)
	{
		std::cout << _pfd_arr.size() - 1 << " connected clients. Waiting for events...\n";
		int poll_result = poll(_pfd_arr.data(), _pfd_arr.size(), -1);
		if (poll_result < 0)
		{
			std::cerr << "poll error: " << strerror(errno) << std::endl;
			break;
		}
		std::cout << poll_result << " event(s)\n";

		for (size_t i = 0; i < _pfd_arr.size(); i++)
		{
			if ((_pfd_arr[i].revents & POLLIN) && _pfd_arr[i].fd == _serv_socket)
				accept_socket();
			else if (_pfd_arr[i].revents & POLLIN)
			{
				try
				{
					client_event(_pfd_arr[i].fd);
				}
				catch(const std::exception& e)
				{
					std::cerr << "There was an error on socket " << _pfd_arr[i].fd << "\n";
					_disconnected_sockets.push_back(_pfd_arr[i].fd);
				}
			}
			else if (_pfd_arr[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				std::cerr << "There was an error on socket " << _pfd_arr[i].fd << "\n";
				_disconnected_sockets.push_back(_pfd_arr[i].fd);
			}

			if (_pfd_arr[i].revents != 0 && !--poll_result)
				break;
		}

		// Add new clients
		if (_accepted_sockets.size() > 0)
			add_clients();
	
		// Close and remove disconnected clients
		if (_disconnected_sockets.size() > 0)
			disconnect_sockets();
	}
}
