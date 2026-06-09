#include "Server.hpp"

void ping(Server &s, Client &c, std::string &line)
{
	(void)s;
	std::string message = ":ircserver PONG " + line;
	c.sendMsg(message);
}
