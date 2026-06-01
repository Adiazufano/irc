#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"

#include <sstream>

void joinMessages(std::string name, std::string _topic, std::vector<int> _clients)
{
    std::string msg;
    msg = "User " + name + "has joined the channel";
    _topic = "The channel's topic is: " + _topic;
    for(std::vector<int>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        print_message(*it, msg);
        if(_topic.empty())
            print_message(*it, _topic);
    }
}


void joinChannel(Client& client, std::string line, std::vector<Channel *> &_channels)
{
    std::istringstream str(line);
    std::string name;

    str >> name;
    if(name[0] != '#' && name[0] != '&')
    {
        std::cout << "Error: channel's name must start with <#> or <&>" << std::endl;
        return;
    }
    std::cout << "Nombre del canal" << name << std::endl;
    for(std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        std::cout << (*it)->getChannelName() << std::endl;
        if((*it)->getChannelName() == name)
        {
            joinMessages(client.getNickname(), (*it)->getChannelTopic(), (*it)->getClientsArray());
            (*it)->addClient(client.getFd());
            return ;
        }
    }
    Channel* ch = new Channel (name, "", "", client.getFd());
    ch->addAdmind(client.getFd());
    _channels.push_back(ch);
    std::cout << "Hemos creado el canal nuevo" << std::endl;
}
