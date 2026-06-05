#include "Server.hpp"

void broadcastUser(Server &s, std::string &name, std::string &text)
{
	std::map<int, Client> clients = s.getClients();
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
	Channel *ch = s.getChannels()[channelName];
	if (ch == NULL)
	{
		// no such channel
		// ERR_NOSUCHCHANNEL (403)
		// "<client> <channel> :No such channel"

	}
	std::vector<int> members = ch->getClientsArray();
	for (int i = 0; i < members.size(); ++i)
	{
		if (members[i] != exclude)
			print_message(members[i], text);
	}
	
}

void privmsg(Server &s, Client &c, std::string &line)
{
	(void)s;
	(void)c;

	std::istringstream iss1(line);
	std::string text;
	std::string targets_full;
	std::vector<std::string> targets;

	getline(iss1, targets_full, ' ');
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
		}
		if (it->at(0) == '#')
		{
			broadcastChannel(s, *it, text, c.getFd());
			//s.broadcastChannel(*it, c.getFd(), text);
		}
		else
		{
			broadcastUser(s, *it, text);
		}
	}
	

}

/* #include <string>
#include <iostream>
#include <sstream>
#include <vector>

int main()
{
	std::string line("manolo,gonzalo      mira que  bonita esta la luna");

	std::istringstream iss1(line);
	std::string targets;
	std::vector<std::string> targets_v;
	std::string text;

	getline(iss1, targets, ' ');
	getline(iss1 >> std::ws, text);
	std::istringstream iss2(targets);
	for (std::string target; getline(iss2, target, ',');)
		targets_v.push_back(target);

	std::cout << "targets: >" << targets << "<" << std::endl;
	std::cout << "text:    >" << text << "<" << std::endl;
	std::vector<std::string>::iterator it;
	for (it = targets_v.begin(); it != targets_v.end(); it++)
		std::cout << *it << std::endl;

	return 0;
}

int main2()
{
	std::string line("manolo,gonzalo      mira que  bonita esta la luna");

	std::istringstream input;
	std::istringstream input2;
	std::string targets;
	std::string text;
	std::vector<std::string> targets_v;

	input.str(line);
	getline(input, targets, ' ');
	input >> std::ws;
	getline(input, text);

	std::cout << "targets: >" << targets << "<" << std::endl;
	std::cout << "text:    >" << text << "<" << std::endl;


	input2.str(targets);
	for (std::string target; getline(input2, target, ',');)
	{
		targets_v.push_back(target);
	}

	std::vector<std::string>::iterator it;
	for (it = targets_v.begin(); it != targets_v.end(); it++)
		std::cout << *it << std::endl;

	return 0;
} */
