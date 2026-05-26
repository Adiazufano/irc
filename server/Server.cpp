#include "Server.hpp"

// Default constructor
Server::Server() {}

// Destructor
Server::~Server()
{
	std::cout << "Closing server...\n";
	close(_serv_socket);
	std::cout << "Closed" << '\n';
}

Server::Server(char *port, char* password)
{
	_serv_socket = -1;
	// MUY IMPORTANTE:
	// Para validar el puerto y la contraseña (pasados por argumentos de programa)
	// hay que modificar este if:
	if (true) //validate arguments
	{
		_port = port;
		_password = password;
	}
}

bool Server::_run_server = true;

void sigint_handler(int signal)
{
	(void)signal;
	std::cout << "Signal " << signal << '\n';
	Server::_run_server = false;
}

void Server::init()
{
	signal(SIGINT, sigint_handler);

	// Network address
	struct addrinfo hints;
	struct addrinfo *res;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;		// localhost

	if (getaddrinfo(0, _port.data(), &hints, &res) < 0)
		throw std::runtime_error("Error getaddrinfo");
	
	// Server socket
	_serv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (_serv_socket < 0)
		throw std::runtime_error("Error socket");

	// TO DO:
	//setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(_serv_socket, res->ai_addr, res->ai_addrlen) < 0)
		throw std::runtime_error("Error bind");

	// TO DO:
	// Si ocurre un error antes hay que llamar a freeaddrinfo de alguna manera
	freeaddrinfo(res);

	struct pollfd serv_pfd = { _serv_socket, POLLIN, 0 };
	_arr.push_back(serv_pfd);

	// Listen
	// To do: ¿Qué número hay que pasar como segundo parámetro?
	if (listen(_serv_socket, 10) <0)
		throw std::runtime_error("Error listen");

	std::cout << "Server listening on port " << _port.data() << '\n';
}

void Server::run()
{
	while (Server::_run_server)
	{
		std::vector<struct pollfd> new_clients;
		std::vector<struct pollfd> disconnected_clients;
		std::cout << "Waiting for events... Known clients: " << _arr.size() << '\n';
		int poll_result = poll(_arr.data(), _arr.size(), -1);
		std::cout << "poll_result: " << poll_result << '\n';
		
		while (poll_result > 0 && Server::_run_server)
		{
			for (size_t i = 0; i < _arr.size(); i++)
			{
				// New incoming conection
				if (_arr[i].revents == POLLIN && _arr[i].fd == _serv_socket)
				{
					int new_client = accept(_serv_socket, 0, 0);
					_clients.insert(std::make_pair(new_client, Client(new_client)));
					struct pollfd pfd = { new_client, POLLIN, 0 };
					new_clients.push_back(pfd);
					std::cout << "New connection accepted: Socket " << new_client << '\n';
				}
				// Event from known client (message or disconnection)
				else if (_arr[i].revents == POLLIN)
				{
					char buffer[512] = { 0 };
					ssize_t n_bytes = recv(_arr[i].fd, buffer, sizeof(buffer), 0);
					if (n_bytes == 0)
					{
						std::cout << "Client at socket " << _arr[i].fd << " disconnected\n";
						disconnected_clients.push_back(_arr[i]);
					}
					else if (n_bytes > 0)
					{
						//buffer acumulativo ara cada cliente
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
							commandParse(mensaje, _clients[_arr[i].fd]);
						}
						//si autentificacionmandarmensajes
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
					// Errors
					else if (_arr[i].revents == POLLERR || _arr[i].revents == POLLHUP || _arr[i].revents == POLLNVAL)
					{
						// TO DO: Handle errors
						std::cout << "There was an error at socket " << _arr[i].fd << "\n";
					}
				}
				poll_result--;
			}
		}

		// Add new clients
		if (new_clients.size() > 0)
		{
			std::cout << "Adding " << new_clients.size() << " new clients" << '\n';
			for (size_t i = 0; i < new_clients.size(); i++)
			{
				_arr.push_back(new_clients[i]);
			}
		}
	
		// Close and remove disconnected clients
		if (disconnected_clients.size() > 0)
		{
			std::cout << "Removing " << disconnected_clients.size() << " disconnected clients" << '\n';
			for (size_t i = 0; i < disconnected_clients.size(); i++)
			{
				std::vector<struct pollfd>::iterator it = _arr.begin();
				while (it != _arr.end())
				{
					if (disconnected_clients[i].fd == it->fd)
					{
						close(it->fd);
						_clients.erase(it -> fd);
						it = _arr.erase(it);
						break;
					}
					it++;
				}
			}
		}
	}
}
