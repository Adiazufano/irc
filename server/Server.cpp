#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

bool Server::run_server = true;

// Default constructor
Server::Server() {}

// Destructor
Server::~Server()
{
	std::cout << "Closing server...\n";
	freeaddrinfo(_addr_lst);
	close(_serv_socket);
	std::cout << "Closed" << std::endl;
}

// Parameterized constructor
Server::Server(char *port, char* password) : _port(port), _password(password), _serv_socket(-1), _addr_lst(NULL)
{}

void sigint_handler(int signal)
{
	(void)signal;
	Server::run_server = false;
}

int command_level(std::string cmd)
{
	if(cmd == "/KICK")
		return(1);
	else if(cmd == "/INVITE")
		return(2);
	else if(cmd == "/TOPIC")
		return(3);
	else if(cmd == "/MODE")
		return(4);
	else if(cmd == "/JOIN")
		return(5);
	else
		return(0);
}

void validate_command(const std::string& cmd, Client &client, std::vector<Channel *> channels)
{
    if (cmd.empty())
        return ;
    std::istringstream str(cmd);
    std::string command;
	std::string line;

	str >> command;
	std::getline(str, line);
	for(unsigned long i = 0; i < command.length(); i++)
		command[i] = std::toupper(command[i]);	
	int level = command_level(command); 

	switch(level)
	{
		case 1:
			std::cout << "Kick him" << std::endl;
			break;
		case 2:
			std::cout << "Invite him" << std::endl;
			break;
		case 3:
			std::cout << "Topic him" << std::endl;
			break;
		case 4:
			std::cout << "Mode him" << std::endl;
			break;
		case 5:
			joinChannel(client, line, channels);
			break;		
		default:
			std::cout << "End him" << std::endl;
			break;
	}
}


void Server::init()
{
	signal(SIGINT, sigint_handler);

	// Network address
	struct addrinfo hints;
	//struct addrinfo *res;

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

	// TO DO:
	setsockopt(_serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	if (bind(_serv_socket, _addr_lst->ai_addr, _addr_lst->ai_addrlen) < 0)
		throw std::runtime_error(strerror(errno));

	struct pollfd serv_pfd = { _serv_socket, POLLIN, 0 };
	_arr.push_back(serv_pfd);

	// Listen
	// To do: ¿Qué número hay que pasar como segundo parámetro?
	if (listen(_serv_socket, 10) <0)
		throw std::runtime_error(strerror(errno));

	std::cout << "Server listening on port " << _port.data() << '\n';
}

void Server::add_new_client_pfd()
{
	int new_client = accept(_serv_socket, 0, 0);
	_clients.insert(std::make_pair(new_client, Client(new_client)));
	struct pollfd pfd = { new_client, POLLIN, 0 };
	_new_clients.push_back(pfd);
	std::cout << "New connection accepted: Socket " << new_client << '\n';
}

void Server::client_event(int i)
{
	char buffer[512] = { 0 };
	ssize_t n_bytes = recv(_arr[i].fd, buffer, sizeof(buffer), 0);
	if (n_bytes == 0)
	{
		std::cout << "Client at socket " << _arr[i].fd << " disconnected\n";
		_disconnected_clients.push_back(_arr[i].fd);
	}
	else if (n_bytes > 0)
	{
		//buffer acumulativo para cada cliente
		_clients[_arr[i].fd].buffer += std::string(buffer, n_bytes);
		std::string &buf = _clients[_arr[i].fd].buffer;
		size_t pos;
		//no considerar mensaje completo hasta encontrar "\r\n"
		while ((pos = buf.find("\r\n")) != std::string::npos)
		{
			std::string mensaje = buf.substr(0, pos);
			buf.erase(0, pos + 2);
			std::cout << "Mensaje completo: " << mensaje << "\n";
			//parseo de comandos de autentificacion
			commandParse(mensaje, _clients[_arr[i].fd], _password);
			// To Do: No hay que dejar validar comandos hasta que no hayamos confirmado correctamente la conexión del usuario.
			validate_command(mensaje, _clients[_arr[i].fd], _channels);
		}
		//si autentificacion mandar mensajes
		Client &cli = _clients[_arr[i].fd];
		if (!cli.getAuthenticated() && !cli.getNickname().empty() && !cli.getUser().empty())
		{
			cli.setAuthenticated(true);
			std::string nick = cli.getNickname();
			print_message(_arr[i].fd, ":my_serv_irc 001 " + nick + " :Welcome to the IRC Network, " + nick);
			print_message(_arr[i].fd, ":my_serv_irc 002 " + nick + " :Your host is my_serv_irc, running version 1.0");
			print_message(_arr[i].fd, ":my_serv_irc 003 " + nick + " :This server was created May 2026");
			print_message(_arr[i].fd, ":my_serv_irc 004 " + nick + " my_serv_irc 1.0 o itkol");
			std::cout << "[SERVER] Bienvenido enviado de forma segura.\n";
		}
	}
}

void Server::handle_errors(int i)
{
	// TO DO: Handle errors
	std::cout << "There was an error at socket " << _arr[i].fd << "\n";
}

void Server::run()
{
	while (Server::run_server)
	{
		//std::vector<struct pollfd> new_clients;
		// std::vector<int> disconnected_clients;
		std::cout << _arr.size() - 1 << " connected clients. Waiting for events...\n";
		int poll_result = poll(_arr.data(), _arr.size(), -1);
		std::cout << "poll_result: " << poll_result << '\n';
		
		while (poll_result > 0 && Server::run_server)
		{
			for (size_t i = 0; i < _arr.size(); i++)
			{
				// New incoming connection
				if (_arr[i].revents == POLLIN && _arr[i].fd == _serv_socket)
					add_new_client_pfd();
				// Event from known client (message or disconnection)
				else if (_arr[i].revents == POLLIN)
					client_event(i);
				// Errors
				else if (_arr[i].revents == POLLERR || _arr[i].revents == POLLHUP || _arr[i].revents == POLLNVAL)
					handle_errors(i);
				poll_result--;
			}
		}

		// Add new clients
		if (_new_clients.size() > 0)
		{
			std::cout << "Adding " << _new_clients.size() << " new clients" << '\n';
			for (size_t i = 0; i < _new_clients.size(); i++)
				_arr.push_back(_new_clients[i]);
			_new_clients.clear();
		}
	
		// Close and remove disconnected clients
		if (_disconnected_clients.size() > 0)
		{
			std::cout << "Removing " << _disconnected_clients.size() << " disconnected clients" << '\n';
			for (size_t i = 0; i < _disconnected_clients.size(); i++)
			{
				std::vector<struct pollfd>::iterator it = _arr.begin();
				while (it != _arr.end())
				{
					if (_disconnected_clients[i] == it->fd)
					{
						close(it->fd); // Mejor llamar a close desde el destructor del cliente??
						_clients.erase(it -> fd);
						it = _arr.erase(it);
						break;
					}
					it++;
				}
			}
			_disconnected_clients.clear();
		}
	}
}
