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
#include "Server.hpp"
#include "Client.hpp"


int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage:   " << argv[0] << " <port> <password>\n";
		std::cout << "Example: " << argv[0] << " 6667 1234\n";
		return (1);
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
