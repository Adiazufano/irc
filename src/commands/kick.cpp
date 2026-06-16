#include "Server.hpp"
#include "replies.hpp"
#include "utils.hpp"
#include <algorithm>

void    cmdKick(Server &s, Client &client, std::string line)
{
    std::istringstream iss(line);
    std::string channelName;
    std::string nick;
    std::string resto;
    iss >> channelName >> nick;

    if (channelName.empty() || nick.empty())
        return (client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickname(), client.getCliCmd())));

    if (!checkName(channelName))
        return (client.sendMsg(ERR_NOSUCHCHANNEL(client.getNickname(), channelName)));

    if(!s.findChannelbyName(channelName))
        return (client.sendMsg(ERR_NOSUCHCHANNEL(client.getNickname(), channelName)));

    Channel *channel = s.getChannelbyName(channelName);
    if (!channel->hasClient(client))
        return(client.sendMsg(ERR_NOTONCHANNEL(client.getNickname(), channelName)));

    if (!channel->isAdmin(client.getFd()))
        return(client.sendMsg(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName)));

    std::map<std::string, int>& clients = s.getClientsByNick();
    std::map<std::string, int>::iterator it = clients.find(nick);
    if (it == clients.end())
        return(client.sendMsg(ERR_USERNOTINCHANNEL(nick, channelName)));

    int targetFd = it->second;

    Client &cli = s.getClientbyFd(targetFd);
    std::vector<int> _fd_clients = channel->getClientsArray();
    bool targetInChannel = std::find(_fd_clients.begin(), _fd_clients.end(), targetFd) != _fd_clients.end();

    if (targetInChannel)
    {
        resto = getMsg(iss);
        if (resto.empty())
            resto = "Kicked out by an operator";

        std::string msgKick = ":" + client.getNickname() + "!" + client.getUser() + "@" + client.getHostname() + " KICK " + channelName + " " + nick + " :" + resto;
        channel->sendMembers(msgKick);
        channel->removeClient(cli.getFd());
        cli.removeChannel(*channel);
    }
    else
    {
        client.sendMsg(ERR_USERNOTINCHANNEL(nick, channelName));
    }
}
