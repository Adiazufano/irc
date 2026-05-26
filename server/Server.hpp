#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <map>
#include <exception>
#include "Client.hpp"
#include <signal.h>

//bool g_run_server;

class Server
{
	private:
		std::string					_port;
		std::string					_password;
		int							_serv_socket;
		std::vector<struct pollfd>	_arr;
		std::map<int, Client>		_clients;
		
	public:
		static bool	_run_server;
		Server();
		Server(char *port, char *password);
		~Server();
		void init();
		void run();
};
	
//void stop_server();
//void sigint_handler(int signal);

#endif
