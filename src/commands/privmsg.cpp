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

void broadcastUser(Server &s, std::string &name, std::string &text)
{
	std::map<int, Client> &clients = s.getClients();
	std::map<int, Client>::iterator it;

	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second.getNickname() == name)
			print_message(it->first, text);
	}
	if (it == clients.end())
	{
		// no such client
		//ERR_NOSUCHNICK (401)
		// "<client> <nickname> :No such nick/channel"
	}
}

void broadcastChannel(Server &s, std::string &channelName, std::string &text, int exclude)
{
	// Check if user is member of the channel
	// ERR_CANNOTSENDTOCHAN (404)
	//"<client> <channel> :Cannot send to channel"

	Channel *ch = s.getChannels()[channelName];
	if (ch == NULL)
	{
		// no such channel
		// ERR_NOSUCHCHANNEL (403)
		// "<client> <channel> :No such channel"
	}
	std::vector<int> members = ch->getClientsArray();
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i] != exclude)
			print_message(members[i], text);
	}
}

void privmsg(Server &s, Client &c, std::string &line)
{
	std::cout << "privmsg: START - - - - - - - \n";
	std::cout << "privmsg: line        : " << line << "\n";

	std::istringstream iss1(line);
	std::string text;
	std::string targets_full;
	std::vector<std::string> targets;

	getline(iss1 >> std::ws, targets_full, ' ');
	getline(iss1 >> std::ws, text);

	std::istringstream iss2(targets_full);
	for (std::string t; getline(iss2, t, ',');)
	{
		targets.push_back(t);
	}

	if (text.empty())
	{
		// ERR_NOTEXTTOSEND (412) "<client> :No text to send"
		std::string message = ":ircserver 412 " + c.getNickname() + " :No text to send";
		print_message(c.getFd(), message);
	}
	for (std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		if (it->empty())
		{
			// ERR_NORECIPIENT (411) "<client> :No recipient given (<command>)"
			std::string message = ":ircserver 411 " + c.getNickname() + " :No recipient given (PRIVMSG)";
			print_message(c.getFd(), message);
			//print_message(c.getFd(), std::string(":ircserver 411 " + c.getNickname() + " :No recipient given (PRIVMSG)"));
		}
		if (it->at(0) == '#')
			broadcastChannel(s, *it, text, c.getFd());
		else
			broadcastUser(s, *it, text);
	}
}
