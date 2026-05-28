#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"

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
#include <signal.h>
#include <iterator>

//bool g_run_server;

class Server
{
	typedef std::vector<Channel *>::iterator channel_iterator;

	private:
		std::string					_port;
		std::string					_password;
		int							_serv_socket;
		std::vector<struct pollfd>	_arr;
		std::vector<Channel *>		_channels;
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
void ft_toupper(std::string &str);


#endif
