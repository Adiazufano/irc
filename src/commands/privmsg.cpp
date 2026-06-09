#include "Server.hpp"

void broadcastUser(Server &s, Client &c, std::string &target, std::string &text)
{
	// Target not found
	if (!s.getClientsByNick().count(target))
	{
		std::string message = ERR_NOSUCHNICK(c.getNickname(), target);
		c.sendMsg(message);
		return;
	}

	// Send private message to target client
	Client &dest = s.getClients()[s.getClientsByNick()[target]];
	std::string message = ":" + c.getNickname() + " PRIVMSG " + target + " :" + text;
	dest.sendMsg(message);
}

void broadcastChannel(Server &s, Client &c, std::string &channelName, std::string &text)
{
	// Channel doesn't exist
	if (!s.getChannels().count(channelName))
	{
		std::string message = ERR_NOTONCHANNEL(c.getNickname(), channelName);
		c.sendMsg(message);
		return;
	}

	// Sender doesn't belong to channel
	Channel *ch = s.getChannels()[channelName];
	if (!ch->hasClient(c))
	{
		std::string message = ERR_CANNOTSENDTOCHAN(c.getNickname(), channelName);
		c.sendMsg(message);
		return;
	}

	// Send message to all channel members except sender
	std::string message = ":" + c.getNickname() + "!" + c.getUser() + "@" + c.getHostname() + " PRIVMSG " + channelName + " " + text;
	ch->sendMembers(s, message, c.getFd());
}

void privmsg(Server &s, Client &c, std::string &line)
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
	{
		std::string message = ERR_NOTEXTTOSEND(c.getNickname());
		c.sendMsg(message);
		return;
	}

	// Iterate through comma separated targets and send message to each one
	std::istringstream iss2(targets_full);
	for (std::string target; getline(iss2, target, ',');)
	{
		// Target is empty
		if (target.empty())
		{
			std::string message = ERR_NORECIPIENT(c.getNickname(), c.getCliCmd());
			c.sendMsg(message);
		}
		else if (target[0] == '#')
			broadcastChannel(s, c, target, text);	// Target is a channel
		else
			broadcastUser(s, c, target, text);		// Target is a user
	}
}
