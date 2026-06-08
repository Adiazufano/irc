#include "Server.hpp"
#include "replies.hpp"

void    commandKick(Server &s, Client &client, std::string line)
{
    std::istringstream iss(line);
    std::string channelName;
    std::string nick;
    std::string resto;
    iss >> channelName >> nick;
    std::getline(iss, resto);

    if (channelName.empty() || nick.empty())
    {
        print_message(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname(), "KICK"));
        return;
    }

    if (!checkName(channelName))
    {
        std::cout << "El nombre del canal no es correcto" << std::endl;
        return ;
    }

    std::map<std::string, Channel*>& channels = s.getChannels(); 
    if (channels.count(channelName) == 0)
    {
        std::cout << "Error: El canal " << channelName << " no existe" << std::endl;
        return;
    }

    if (!s.getChannels().count(channelName))
	{
		// ERR_NOSUCHCHANNEL (403) "<client> <channel> :No such channel"
		print_message(client.getFd(), ERR_NOSUCHCHANNEL(nick, channelName));
		return;
	}

	Channel *channel = channels[channelName];
    if (!channel->hasClient(client))
    {
        // "<client> <channel> :You're not on that channel"
        print_message(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
        return;
    }

    if (!s.getChannels()[channelName]->isAdmin(client.getFd()))
    {
        print_message(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName));
        return;
    }

    std::map<std::string, int>& clients = s.getClientsByNick();
    std::map<std::string, int>::iterator it = clients.find(nick);
    if (it == clients.end())
    {
        print_message(client.getFd(), ERR_USERNOTINCHANNEL(nick, channelName));
        return;
    }
	Client &cli = s.getClients()[s.getClientsByNick()[nick]];
    std::vector<int> _fd_clients = channel->getClientsArray();
    if (channel -> hasClient(cli))
    {
        channel -> removeClient(cli);
        if (!resto.empty() && resto[0] == ' ')
            resto.erase(0, 1);
        if (!resto.empty() && resto[0] == ':')
            resto.erase(0, 1);
        while (!resto.empty() && resto[0] == ' ')
            resto.erase(0, 1);
        if (resto.empty())
            resto = "Expulsado por un operador";
        std::string msgKick = ":" + client.getNickname() + " KICK " + channelName + " " + nick + " :" + resto + "\r\n";
        userMessages(s, cli, msgKick);
        channel->removeClient(cli);
    }
    else
    {
        print_message(client.getFd(), ERR_USERNOTINCHANNEL(nick, channelName));
    }
}

