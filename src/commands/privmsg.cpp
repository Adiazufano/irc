#include "Server.hpp"

/*
# define ERR_NOSUCHNICK "401"
# define ERR_CANNOTSENDTOCHAN "404"
# define ERR_NOSUCHCHANNEL "403"
# define ERR_NORECIPIENT "411"
# define ERR_NOTEXTTOSEND "412"

# define ERR_NOSUCHNICK(client, target) ("401 " + client + " " + target + " :No such nick/channel\r\n")
# define ERR_NORECIPIENT(client) ("411 " + client + " :No recipient given PRIVMSG\r\n")
# define ERR_NOTEXTTOSEND(client) ("412 " + client + " :No text to send\r\n")
# define RPL_PRIVMSG(nick, username, target, message) (":" + nick + "!" + username + "@localhost PRIVMSG " + target + " " + message + "\r\n")
*/

//:Angel PRIVMSG Wiz :Hello are you receiving this message ?
//                                  ; Message from Angel to Wiz.

void broadcastUser(Server &s, Client &c, std::string &target, std::string &text)
{
	std::map<int, Client> &clients = s.getClients();
	std::map<int, Client>::iterator it;

	// Iterate through clients map to find target
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname() == target)
		{
			std::string message = ":" + c.getNickname() + " PRIVMSG " + target + " :" + text;
			std::cout << "Sending message to socket " << it->first << " " << message << "\n";
			print_message(it->first, message);
			return;
		}
	}

	// Target not found ERR_NOSUCHNICK (401) "<client> <nickname> :No such nick/channel"
	std::string message = ":ircserver 401 " + c.getNickname() + " " + target + " :No such nick/channel";
	print_message(c.getFd(), message);
}

void broadcastChannel(Server &s, Client &c, std::string &channelName, std::string &text)
{
	// Channel doesn't exist
	if (!s.getChannels().count(channelName))
	{
		// ERR_NOSUCHCHANNEL (403) "<client> <channel> :No such channel"
		std::string message = ":ircserver 403 " + c.getNickname() + " " + channelName + " :No such channel";
		print_message(c.getFd(), message);
		return;
	}

	// Sender doesn't belong to channel
	Channel *ch = s.getChannels()[channelName];
	if (!ch->hasClient(c))
	{
		// ERR_CANNOTSENDTOCHAN (404) "<client> <channel> :Cannot send to channel"
		std::string message = ":ircserver 404 " + c.getNickname() + " " + channelName + " :Cannot send to channel";
		print_message(c.getFd(), message);
		return;
	}

	// Send message to all channel members except sender
	std::vector<int> members = ch->getClientsArray();
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i] == c.getFd())
			continue;
		std::string message = ":" + c.getNickname() + "!" + c.getUser() + "@" + c.getHostname() + " PRIVMSG " + channelName + " " + s.getClients()[i].getNickname() + text;
		print_message(members[i], message);
	}
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
		// ERR_NOTEXTTOSEND (412) "<client> :No text to send"
		std::string message = ":ircserver 412 " + c.getNickname() + " :No text to send";
		print_message(c.getFd(), message);
		return;
	}

	// Iterate through comma separated targets and send message to each one
	std::istringstream iss2(targets_full);
	for (std::string target; getline(iss2, target, ',');)
	{
		// Target is empty
		if (target.empty())
		{
			// ERR_NORECIPIENT (411) "<client> :No recipient given (<command>)"
			std::string message = ":ircserver 411 " + c.getNickname() + " :No recipient given (PRIVMSG)";
			print_message(c.getFd(), message);
			return;
		}
		// Target is a channel
		if (target[0] == '#')
			broadcastChannel(s, c, target, text);
		// Target is a user
		else
			broadcastUser(s, c, target, text);
	}
}
