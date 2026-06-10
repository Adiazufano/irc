#include "Server.hpp"

void ping(Server &s, Client &client, std::string &line)
{
	(void)s;
	std::string message = ":ircserver PONG " + line;
	client.sendMsg(message);
}
