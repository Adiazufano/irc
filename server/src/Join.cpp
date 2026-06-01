#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"

#include <sstream>


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
            std::cout << "Nos unimos al canal" << std::endl;
            (*it)->addClient(client.getFd());
            return ;
        }
    }
    Channel* ch = new Channel (name, "default", "default", client.getFd());
    ch->addAdmind(client.getFd());
    _channels.push_back(ch);
    std::cout << "Hemos creado el canal nuevo" << std::endl;
}
