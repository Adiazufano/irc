#include "Server.hpp"
#include "replies.hpp"
#include <algorithm>

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
        return;
    }

    std::map<std::string, Channel*>& channels = s.getChannels();
    if (!s.getChannels().count(channelName))
    {
        print_message(client.getFd(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
        return;
    }

    Channel *channel = channels[channelName];
    if (!channel->hasClient(client))
    {
        print_message(client.getFd(), ERR_NOTONCHANNEL(client.getNickname(), channelName));
        return;
    }
    if (!channel->isAdmin(client.getFd()))
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

    int targetFd = it->second;
    if (s.getClients().count(targetFd) == 0)
    {
        print_message(client.getFd(), ERR_USERNOTINCHANNEL(nick, channelName));
        return;
    }

    Client &cli = s.getClients()[targetFd];
    std::vector<int> _fd_clients = channel->getClientsArray();
    bool targetInChannel = std::find(_fd_clients.begin(), _fd_clients.end(), targetFd) != _fd_clients.end();

    if (targetInChannel)
    {
        if (!resto.empty() && resto[0] == ' ')
            resto.erase(0, 1);
        if (!resto.empty() && resto[0] == ':')
            resto.erase(0, 1);
        while (!resto.empty() && resto[0] == ' ')
            resto.erase(0, 1);
        if (resto.empty())
            resto = "Expulsado por un operador";

        std::string msgKick = ":" + client.getNickname() + "!" + client.getUser() + "@" + client.getHostname() + " KICK " + channelName + " " + nick + " :" + resto;
        for (size_t i = 0; i < _fd_clients.size(); ++i)
            print_message(_fd_clients[i], msgKick);
        channel->removeClient(cli.getFd());
        cli.removeChannel(*channel);
    }
    else
    {
        print_message(client.getFd(), ERR_USERNOTINCHANNEL(nick, channelName));
    }
}