#ifndef SERVER_HPP
#define SERVER_HPP

#include "../include/Channel.hpp"
#include "../include/Client.hpp"

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

class Channel;

class Client;

class Server
{
	typedef std::vector<Channel *>::iterator channel_iterator;

	private:
		std::string					_port;
		std::string					_password;
		int							_serv_socket;
		struct addrinfo				*_addr_lst;
		std::vector<struct pollfd>	_pfd_arr;
		std::vector<int>			_accepted_clients;
		std::vector<int>			_disconnected_clients;
		std::map<std::string, Channel *>		_channels;
		std::map<int, Client>		_clients;
		void						accept_client();
		void						client_event(int i);
		void						handle_errors(int i);
		void						add_clients();
		void						disconnect_clients();

	public:
		static bool	run_server;
		Server();
		Server(char *port, char *password);
		~Server();
		void init();
		void run();
		std::map<std::string, Channel *> &getChannels();
		std::map<int, Client> &getClients();

};
	
//void stop_server();
//void sigint_handler(int signal);
void ft_toupper(std::string &str);


#endif
