#include "Server.hpp"

void broadcastUser(Server &s, Client &client, std::string &target, std::string &text)
{
	// Target not found
	if (!s.getClientsByNick().count(target))
		return client.sendMsg(ERR_NOSUCHNICK(client.getNickname(), target));

	// Send private message to target client
	Client &dest = s.getClients()[s.getClientsByNick()[target]];
	//std::string message = ":" + client.getNickname() + " PRIVMSG " + target + " :" + text;
	dest.sendMsg(":" + client.getNickname() + " PRIVMSG " + target + " :" + text);
}

void broadcastChannel(Server &s, Client &client, std::string &channelName, std::string &text)
{
	// Channel doesn't exist
	if (!s.getChannels().count(channelName))
		return client.sendMsg(ERR_NOTONCHANNEL(client.getNickname(), channelName));

	// Sender doesn't belong to channel
	Channel *ch = s.getChannels()[channelName];
	if (!ch->hasClient(client))
		return client.sendMsg(ERR_CANNOTSENDTOCHAN(client.getNickname(), channelName));

	// Send message to all channel members except sender
	std::string message = ":" + client.getNickname() + "!" + client.getUser() + "@" + client.getHostname() + " PRIVMSG " + channelName + " " + text;
	ch->sendMembers(s, message, client.getFd());
}

void privmsg(Server &s, Client &client, std::string &line)
{
	// Parse targets and text
	std::istringstream iss1(line);
	std::string text;
	std::string targets_full;
	std::vector<std::string> targets;

	getline(iss1 >> std::ws, targets_full, ' ');
	getline(iss1 >> std::ws, text);

	// Message is empty
	if (text.empty())
		return client.sendMsg(ERR_NOTEXTTOSEND(client.getNickname()));

	// Iterate through comma separated targets and send message to each one
	std::istringstream iss2(targets_full);
	for (std::string target; getline(iss2, target, ',');)
	{
		// Target is empty
		if (target.empty())
			client.sendMsg(ERR_NORECIPIENT(client.getNickname(), client.getCliCmd()));
		else if (target[0] == '#')
			broadcastChannel(s, client, target, text);	// Target is a channel
		else
			broadcastUser(s, client, target, text);		// Target is a user
	}
}
