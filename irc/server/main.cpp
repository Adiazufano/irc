#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <signal.h>
#include "Cliente.hpp"
#include <map>

#define PORT "6669"

bool g_run_server = true;

void sigint_handler(int signal)
{
	(void)signal;
	// std::cout << "Signal " << signal << '\n';
	g_run_server = false;
}

int	main()
{
	struct addrinfo hints;
	struct addrinfo *res;

	signal(SIGINT, &sigint_handler);

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;		// localhost

	getaddrinfo(0, PORT, &hints, &res);
	
	int serv_socket;
	serv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	/* `SO_REUSEADDR` is missing
	Without `setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))`
	before `bind`, restarting your server quickly during testing will give you
	"address already in use" errors. */

	bind(serv_socket, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	std::vector<struct pollfd> arr;
	//mapa fd clientes
	std::map<int, Cliente> clientes;
	struct pollfd serv_pfd = { serv_socket, POLLIN, 0 };
	arr.push_back(serv_pfd);
	listen(serv_socket, 10);
	std::cout << "Server listening on port " << PORT << '\n';
	while (g_run_server)
	{
		std::vector<struct pollfd> new_clients;
		std::vector<struct pollfd> disconnected_clients;
		std::cout << "Waiting for events...\n";
		int poll_result = poll(arr.data(), arr.size(), -1);
		std::cout << "poll_result: " << poll_result << '\n';
		if (poll_result > 0)
		{
			for (size_t i = 0; i < arr.size(); i++)
			{
				if (arr[i].revents != 0)
				{
					if (arr[i].fd == serv_socket)
					{
						int new_client = accept(serv_socket, 0, 0);
						clientes.insert(std::make_pair(new_client, Cliente(new_client)));
						struct pollfd pfd = { new_client, POLLIN, 0 };
						new_clients.push_back(pfd);
						std::cout << "New connection accepted: Socket " << new_client << '\n';
					}
					else
					{
						// Gestión de errores:
						// https://github.com/cirosantilli/cpp-cheat/blob/6cc4af959b0ad0d90f2de6af51911315cf4497f7/posix/interactive/poll.c
						// https://stackoverflow.com/questions/24791625/how-to-handle-the-linux-socket-revents-pollerr-pollhup-and-pollnval
						if (arr[i].revents == POLLERR || arr[i].revents == POLLHUP || arr[i].revents == POLLNVAL)
						{
							std::cout << "Client at socket " << arr[i].fd << " disconnected (POLLERR, POLLNVAL or POLLHUP)\n";
							disconnected_clients.push_back(arr[i]);
						}
						else
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
								clientes[arr[i].fd].buffer += std::string(buffer, n_bytes);
								std::string &buf = clientes[arr[i].fd].buffer;
								size_t pos;
								//no considerar mensaje completo hasta encontrar "\r\n"
								while ((pos = buf.find("\r\n")) != std::string::npos)
								{
									std::string mensaje = buf.substr(0, pos);
									buf.erase(0, pos + 2);
									std::cout << "Mensaje completo: " << mensaje << "\n";
									//parseo de comandos de autentificacion
									commandParse(mensaje, clientes[arr[i].fd]);
								}

								//si autentificacionmandarmensajes
								Cliente &cli = clientes[arr[i].fd];
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
						}
					}
					poll_result--;
					if (!poll_result)
						break;
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
						clientes.erase(it -> fd);
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
