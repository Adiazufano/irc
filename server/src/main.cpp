#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <map>
#include <signal.h>
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

int validate_args(char **argv)
{
	//validate port
	std::stringstream ss(argv[1]);
	int port;
	char leftover;

	if (!(ss >> port) || ss >> leftover)
		return 0;
	if (port < 0 || port > 65535)
		return 0;

	size_t index = 0;
	std::string pass(argv[2]);
	while (index < pass.length())
	{
		if (isspace(pass[index]))
			return 0;
		index++;
	}
	std::cout << port << std::endl;
	return 1;
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage:   " << argv[0] << " <port> <password>\n";
		std::cout << "Example: " << argv[0] << " 6667 1234\n";
		return (1);
	}

	if (!validate_args(argv))
	{
		std::cout << "error" << std::endl;
		return 1;
	}

	try
	{
		Server serv(argv[1], argv[2]);
		serv.init();
		serv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}
