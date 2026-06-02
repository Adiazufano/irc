#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"

void channelTopic(std::string line, std::map<std::string, Channel *> &_channels, Client &client)
{
    std::istringstream iss(line);
    std::string channelName;
    iss >> channelName;
    std::string resto;
    std::string topic;
    std::getline(iss, resto);
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
    {
        std::cout << "El canal no existe" << std::endl;
        return;
    }

    Channel &channel = (*it -> second);
    std::vector<int> clients = channel.getClientsArray();
    for (size_t i  = 0; i < clients.size(); i++)
    {
        size_t index = 0;
        int n_pos = 0;
        bool flag = 0;
        while (index < resto.length())
        {
            if (isspace(resto[index]) && flag == 0)
                n_pos++;
            else
                flag = 1;
            index++;
        }
        topic = resto.substr(n_pos);
        if (client.getFd() == clients[i])
            channel.setChannelTopic(topic);
        else
            print_message(client.getFd(), "El cliente no esta en el canal");
    }
    std::cout << channel.getChannelName() <<" TOPIC: "<<channel.getChannelTopic() << std::endl;
}
