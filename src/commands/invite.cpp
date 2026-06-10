#include "Server.hpp"

void invite(Server &s, Client &client, std::string &line)
{
	// Parse nickname and channel
	std::istringstream iss(line);
	std::string nickname;
	std::string channelName;

	iss >> nickname >> channelName;

	// Missing parameters
	if (nickname.empty() || channelName.empty())
		return client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickname(), client.getCliCmd()));

	// Nickname doesn't exist
	if (!s.getClientsByNick().count(nickname))
		return client.sendMsg(ERR_NOSUCHNICK(client.getNickname(), nickname));

	// Channel doesn't exist
	if (!s.getChannels().count(channelName))
		return client.sendMsg(ERR_NOSUCHCHANNEL(client.getNickname(), channelName));

	// Sender doesn't belong to channel
	Channel *ch = s.getChannels()[channelName];
	if (!ch->hasClient(client))
		return client.sendMsg(ERR_CANNOTSENDTOCHAN(client.getNickname(), channelName));

	// Servers MAY reject the command with the ERR_CHANOPRIVSNEEDED numeric.
	// In particular, they SHOULD reject it when the channel has invite-only mode set, and the user is not a channel operator.

	// Target is already on the channel
	Client &target = s.getClients()[s.getClientsByNick()[nickname]];
	if (ch->hasClient(target))
		return client.sendMsg(ERR_USERONCHANNEL(client.getNickname(), nickname, channelName));

	client.sendMsg(RPL_INVITING(client.getNickname(), target.getNickname(), channelName));
	target.sendMsg(INVITE_MSG(client.getNickname(), client.getUser(), client.getHostname(), target.getNickname(), channelName));
}
