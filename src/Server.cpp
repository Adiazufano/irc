#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "commands.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

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

std::map<std::string, Client> &Server::getClientsByNick()
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

	struct pollfd serv_pfd = { _serv_socket, POLLIN, 0 };
	_pfd_arr.push_back(serv_pfd);

	// Listen
	if (listen(_serv_socket, SOMAXCONN) < 0)
		throw std::runtime_error(strerror(errno));

	std::cout << "Server listening on port " << _port.c_str() << '\n';
}

void Server::accept_client()
{
	int fd = accept(_serv_socket, 0, 0);
	if (fd > 0)
	{
		_accepted_clients.push_back(fd);
		std::cout << "New connection accepted: Socket " << fd << '\n';
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
		_disconnected_clients.push_back(_pfd_arr[i].fd);
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
			if (!cli.getAuthenticated())
				commandParse(mensaje, cli, _password, *this);
			// To Do: No hay que dejar validar comandos hasta que no hayamos confirmado correctamente la conexión del usuario.
			else
				validate_command(*this, mensaje, cli);
		}
		//si autentificacion mandar mensajes
		if (cli.getHasPass() && !cli.getNickname().empty() && !cli.getUser().empty() && cli.getAuthenticated() && !cli.getRegistered())
		{
			std::string nick = cli.getNickname();
			print_message(_pfd_arr[i].fd, ":my_serv_irc 001 " + nick + " :Welcome to the IRC Network, " + nick);
			print_message(_pfd_arr[i].fd, ":my_serv_irc 002 " + nick + " :Your host is my_serv_irc, running version 1.0");
			print_message(_pfd_arr[i].fd, ":my_serv_irc 003 " + nick + " :This server was created May 2026");
			print_message(_pfd_arr[i].fd, ":my_serv_irc 004 " + nick + " my_serv_irc 1.0 o itkol");
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
	std::cout << "Adding " << _accepted_clients.size() << " new clients" << '\n';
	for (size_t i = 0; i < _accepted_clients.size(); i++)
	{
		struct pollfd pfd = { _accepted_clients[i], POLLIN, 0 };
		_pfd_arr.push_back(pfd);
		_clients.insert(std::make_pair(_accepted_clients[i], Client(_accepted_clients[i])));
	}
	_accepted_clients.clear();
}

void Server::disconnect_clients()
{
	std::cout << "Removing " << _disconnected_clients.size() << " disconnected clients" << '\n';
	for (size_t i = 0; i < _disconnected_clients.size(); i++)
	{
		std::vector<struct pollfd>::iterator it = _pfd_arr.begin();
		while (it != _pfd_arr.end())
		{
			if (_disconnected_clients[i] == it->fd)
			{
				close(it->fd); // Mejor llamar a close desde el destructor del cliente??
				_clients.erase(it -> fd);
				it = _pfd_arr.erase(it);
				break;
			}
			it++;
		}
	}
	_disconnected_clients.clear();
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
				accept_client();
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
		if (_accepted_clients.size() > 0)
			add_clients();
	
		// Close and remove disconnected clients
		if (_disconnected_clients.size() > 0)
			disconnect_clients();
	}
}
