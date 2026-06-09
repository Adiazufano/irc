#include "Server.hpp"

void invite(Server &s, Client &c, std::string &line)
{
	// Parse nickname and channel
	std::istringstream iss(line);
	std::string nickname;
	std::string channelName;

	iss >> nickname >> channelName;

	// Missing parameters
	if (nickname.empty() || channelName.empty())
	{
		std::string message = ERR_NEEDMOREPARAMS(c.getNickname(), c.getCliCmd());
		c.sendMsg(message);
		return;
	}

	// Nickname doesn't exist
	if (!s.getClientsByNick().count(nickname))
	{
		std::string message = ERR_NOSUCHNICK(c.getNickname(), nickname);
		c.sendMsg(message);
		return;
	}

	// Channel doesn't exist
	if (!s.getChannels().count(channelName))
	{
		std::string message = ERR_NOSUCHCHANNEL(c.getNickname(), channelName);
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

	// Servers MAY reject the command with the ERR_CHANOPRIVSNEEDED numeric.
	// In particular, they SHOULD reject it when the channel has invite-only mode set, and the user is not a channel operator.

	// Target is already on the channel
	Client &target = s.getClients()[s.getClientsByNick()[nickname]];
	if (ch->hasClient(target))
	{
		std::string message = ERR_USERONCHANNEL(c.getNickname(), nickname, channelName);
		c.sendMsg(message);
		return;
	}

	std::string reply = RPL_INVITING(c.getNickname(), target.getNickname(), channelName);
	c.sendMsg(reply);

	std::string message = INVITE_MSG(c.getNickname(), c.getUser(), c.getHostname(), target.getNickname(), channelName);
	target.sendMsg(message);
}
