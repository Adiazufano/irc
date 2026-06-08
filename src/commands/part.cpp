#include "Server.hpp"
#include "Client.hpp"
#include "replies.hpp"

std::vector<std::string> getChannels(Client& client, std::string line)
{
    std::istringstream str(line);
    std::vector<std::string> channels;
    std::string token;

    while(std::getline(str, token, ','))
    {
        if(token.empty() || token[0] != '#')
        {
            print_message(client.getFd(), ERR_NEEDMOREPARAMS(client.getNickname(), client.getCliCmd()));
            return std::vector<std::string>();
        }
        channels.push_back(token);
    }
    return channels;
}

void partChannel(Server &s, Client& client, std::string line)
{
    std::vector<std::string> channels;
    std::string nick = client.getNickname();

    channels = getChannels(client, line);
    for(std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        std::string name = *it;

        if (!s.getChannels().count(name))
        {
            print_message(client.getFd(), ERR_NOSUCHCHANNEL(nick, name));
            continue;
        }
        if (!s.getChannels()[name]->hasClient(client))
        {
            print_message(client.getFd(), ERR_NOTONCHANNEL(nick, name));
            continue;
        }
        std::string msg = ":" + nick + "!" + client.getUser() + "@" + client.getHostname()
                        + " " + client.getCliCmd() + " " + name;

        std::vector<int> clients = s.getChannels()[name]->getClientsArray();
        for(std::vector<int>::iterator jt = clients.begin(); jt != clients.end(); ++jt)
            print_message(*jt, msg);

        s.getChannels()[name]->removeClient(client.getFd());
        client.removeChannel(*s.getChannels()[name]);
    }
}
