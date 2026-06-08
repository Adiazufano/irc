#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "commands.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

void privmsg(Server &s, Client &c, std::string &line);

extern bool run_server;

// Default constructor
Server::Server() {}

// Destructor
Server::~Server()
{
	std::cout << "\nClosing server...\n";
	freeaddrinfo(_addr_lst);
	close(_serv_socket);
	std::cout << "Closed" << std::endl;
	std::map<std::string, Channel*>::iterator it;
	for(it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	_channels.clear();
}

// Parameterized constructor
Server::Server(char *port, char* password) : _port(port), _password(password), _serv_socket(-1), _addr_lst(NULL)
{}

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

void Server::init()
{
	signal(SIGINT, sigint_handler);

	// Network address
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;		// localhost
	int opt = 1;

	if (getaddrinfo(0, _port.data(), &hints, &_addr_lst) < 0)
		throw std::runtime_error(strerror(errno));

	// Server socket
	_serv_socket = socket(_addr_lst->ai_family, _addr_lst->ai_socktype, _addr_lst->ai_protocol);
	if (_serv_socket < 0)
		throw std::runtime_error(strerror(errno));
	if (setsockopt(_serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error(strerror(errno));
	if (bind(_serv_socket, _addr_lst->ai_addr, _addr_lst->ai_addrlen) < 0)
		throw std::runtime_error(strerror(errno));
	
	char hostname[256];
	gethostname(hostname, sizeof(hostname));
	_hostname = hostname;

	struct pollfd serv_pfd = { _serv_socket, POLLIN, 0 };
	_pfd_arr.push_back(serv_pfd);

	// Listen
	if (listen(_serv_socket, SOMAXCONN) < 0)
		throw std::runtime_error(strerror(errno));

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

void Server::client_event(int i)
{
	char buffer[512] = { 0 };
	ssize_t n_bytes = recv(_pfd_arr[i].fd, buffer, sizeof(buffer), 0);
	if (n_bytes < 0)
	{
		// TO DO: Handle recv error
		std::cout << "recv error: " << strerror(errno) << std::endl;
	}
	else if (n_bytes == 0)
	{
		std::cout << "Client at socket " << _pfd_arr[i].fd << " disconnected\n";
		_disconnected_sockets.push_back(_pfd_arr[i].fd);
	}
	else if (n_bytes > 0)
	{
		//buffer acumulativo para cada cliente
		Client &cli = _clients[_pfd_arr[i].fd];
		cli.buffer += std::string(buffer, n_bytes);
		std::string &buf = cli.buffer;
		size_t pos;
		//no considerar mensaje completo hasta encontrar "\r\n"
		while ((pos = buf.find("\r\n")) != std::string::npos)
		{
			std::string mensaje = buf.substr(0, pos);
			buf.erase(0, pos + 2);
			std::cout << "Mensaje completo: " << mensaje << "\n";
			//parseo de comandos de autentificacion
			if (!cli.getRegistered())
				commandParse(mensaje, cli, _password, *this);
			// To Do: No hay que dejar validar comandos hasta que no hayamos confirmado correctamente la conexión del usuario.
			else
				validate_command(*this, mensaje, cli);
		}

		//si autentificacion mandar mensajes
		if (cli.getHasPass() && !cli.getNickname().empty() && !cli.getUser().empty() && cli.getAuthenticated() && !cli.getRegistered())
		{
			cli.setAuthenticated(true);
			std::string nick = cli.getNickname();
			print_message(_pfd_arr[i].fd, ":my_serv_irc 001 " + nick + " :Welcome to the IRC Network, " + nick);
			print_message(_pfd_arr[i].fd, ":my_serv_irc 002 " + nick + " :Your host is my_serv_irc, running version 1.0");
			print_message(_pfd_arr[i].fd, ":my_serv_irc 003 " + nick + " :This server was created May 2026");
			print_message(_pfd_arr[i].fd, ":my_serv_irc 004 " + nick + " my_serv_irc 1.0 o itkol");
			print_message(_pfd_arr[i].fd, ":my_serv_irc 376 " + nick + " :End of /MOTD command.");
			std::cout << "[SERVER] Bienvenido enviado de forma segura.\n";
			cli.setRegistered(true);
		}
	}
}

void Server::handle_errors(int i)
{
	// TO DO: Handle errors
	std::cout << "There was an error at socket " << _pfd_arr[i].fd << "\n";
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

std::string Server::getHostname() const
{
    return _hostname;
}

void Server::disconnect_sockets()
{
	std::cout << "Removing " << _disconnected_sockets.size() << " disconnected clients" << '\n';
	for (size_t i = 0; i < _disconnected_sockets.size(); i++)
	{
		std::vector<struct pollfd>::iterator it = _pfd_arr.begin();
		while (it != _pfd_arr.end())
		{
			if (_disconnected_sockets[i] == it->fd)
			{
				close(it->fd); // Mejor llamar a close desde el destructor del cliente??
				_clients.erase(it -> fd);
				it = _pfd_arr.erase(it);
				break;
			}
			it++;
		}
	}
	_disconnected_sockets.clear();
}

void Server::run()
{
	while (run_server)
	{
		std::cout << _pfd_arr.size() - 1 << " connected clients. Waiting for events...\n";
		int poll_result = poll(_pfd_arr.data(), _pfd_arr.size(), -1);
		if (poll_result < 0)
			break;
		std::cout << "Events: " << poll_result << std::endl;

		for (size_t i = 0; i < _pfd_arr.size(); i++)
		{
			// New incoming connection
			if (_pfd_arr[i].revents == POLLIN && _pfd_arr[i].fd == _serv_socket)
				accept_socket();
			// Event from known client (message or disconnection)
			else if (_pfd_arr[i].revents == POLLIN)
				client_event(i);
			// Errors
			else if (_pfd_arr[i].revents == POLLERR || _pfd_arr[i].revents == POLLHUP || _pfd_arr[i].revents == POLLNVAL)
				handle_errors(i);
			// Break loop if no more events
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
