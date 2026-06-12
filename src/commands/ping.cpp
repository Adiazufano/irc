#include "Server.hpp"

void ping(Client &client, std::string &line)
{
	if (line.empty())
		return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickname(), client.getCliCmd()));

	client.sendMsg(PONG_MSG(line));
}
