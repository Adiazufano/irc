#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <signal.h>
#include "Client.hpp"
#include <map>

bool g_run_server = true;

void sigint_handler(int signal)
{
	(void)signal;
	// std::cout << "Signal " << signal << '\n';
	g_run_server = false;
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage:   " << argv[0] << " <port> <password>\n";
		std::cout << "Example: " << argv[0] << " 6667 1234\n";
		return (1);
	}

	// TO DO: Validate program arguments

	// Signals
	signal(SIGINT, &sigint_handler);

	// Network address
	struct addrinfo hints;
	struct addrinfo *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;		// localhost
	getaddrinfo(0, argv[1], &hints, &res);
	
	// Server socket
	int serv_socket;
	serv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// TO DO:
	//setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bind(serv_socket, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	// Array of pollfds (server socket + client sockets)
	std::vector<struct pollfd> arr;
	//mapa fd clients
	std::map<int, Client> clients;
	struct pollfd serv_pfd = { serv_socket, POLLIN, 0 };
	arr.push_back(serv_pfd);

	// Listen
	listen(serv_socket, 10);
	std::cout << "Server listening on port " << argv[1] << '\n';

	// Server loop
	while (g_run_server)
	{
		std::vector<struct pollfd> new_clients;
		std::vector<struct pollfd> disconnected_clients;
		std::cout << "Waiting for events...\n";
		int poll_result = poll(arr.data(), arr.size(), -1);
		std::cout << "poll_result: " << poll_result << '\n';

		while (poll_result > 0 && g_run_server)
		{
			for (size_t i = 0; i < arr.size(); i++)
			{
					// New incoming conection
					if (arr[i].revents == POLLIN && arr[i].fd == serv_socket)
					{
						int new_client = accept(serv_socket, 0, 0);
						clients.insert(std::make_pair(new_client, Client(new_client)));
						struct pollfd pfd = { new_client, POLLIN, 0 };
						new_clients.push_back(pfd);
						std::cout << "New connection accepted: Socket " << new_client << '\n';
					}
					// Event from known client (message or disconnection)
					else if (arr[i].revents == POLLIN)
					{
						char buffer[512] = { 0 };
						ssize_t n_bytes = recv(arr[i].fd, buffer, sizeof(buffer), 0);
						if (n_bytes == 0)
						{
							std::cout << "Client at socket " << arr[i].fd << " disconnected\n";
							disconnected_clients.push_back(arr[i]);
						}
						else if (n_bytes > 0)
						{
							//buffer acumulativo ara cada cliente
							clients[arr[i].fd].buffer += std::string(buffer, n_bytes);
							std::string &buf = clients[arr[i].fd].buffer;
							size_t pos;
							//no considerar mensaje completo hasta encontrar "\r\n"
							while ((pos = buf.find("\r\n")) != std::string::npos)
							{
								std::string mensaje = buf.substr(0, pos);
								buf.erase(0, pos + 2);
								std::cout << "Mensaje completo: " << mensaje << "\n";
								//parseo de comandos de autentificacion
								commandParse(mensaje, clients[arr[i].fd]);
							}
							//si autentificacionmandarmensajes
							Client &cli = clients[arr[i].fd];
							if (!cli.getAuthenticated() && !cli.getNickname().empty() && !cli.getUser().empty())
							{
								cli.setAuthenticated(true);
								
								std::string nick = cli.getNickname();
								print_message(arr[i].fd, ":my_serv_irc 001 " + nick + " :Welcome to the IRC Network, " + nick);

								print_message(arr[i].fd, ":my_serv_irc 002 " + nick + " :Your host is my_serv_irc, running version 1.0");
								
								print_message(arr[i].fd, ":my_serv_irc 003 " + nick + " :This server was created May 2026");
								
								print_message(arr[i].fd, ":my_serv_irc 004 " + nick + " my_serv_irc 1.0 o itkol");
								std::cout << "[SERVER] Bienvenido enviado de forma segura.\n";
							}
						}
						// Errors
						else if (arr[i].revents == POLLERR || arr[i].revents == POLLHUP || arr[i].revents == POLLNVAL)
						{
							// TO DO: Handle errors
							std::cout << "There was an error at socket " << arr[i].fd << "\n";
						}
						poll_result--;
				}
			}
		}

		// Add new clients
		if (new_clients.size() > 0)
		{
			std::cout << "Adding " << new_clients.size() << " new clients" << '\n';
			for (size_t i = 0; i < new_clients.size(); i++)
			{
				arr.push_back(new_clients[i]);
			}
		}

		// Close and remove disconnected clients
		if (disconnected_clients.size() > 0)
		{
			std::cout << "Removing " << disconnected_clients.size() << " disconnected clients" << '\n';
			for (size_t i = 0; i < disconnected_clients.size(); i++)
			{
				std::vector<struct pollfd>::iterator it = arr.begin();
				while (it != arr.end())
				{
					if (disconnected_clients[i].fd == it->fd)
					{
						close(it->fd);
						clients.erase(it -> fd);
						it = arr.erase(it);
						break;
					}
					it++;
				}
			}
		}
	}

	std::cout << "Closing server...\n";
	close(serv_socket);
	std::cout << "Closed" << '\n';

	return (0);
}
