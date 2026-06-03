#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"

void channelTopic(Server &s, Client &client, std::string line)
{
    std::istringstream iss(line);
    std::string channelName;
    iss >> channelName;
    std::string resto;
    std::string topic;
    std::getline(iss, resto);
    std::map<std::string, Channel*>::iterator it = s.getChannels().find(channelName);
    if (it == s.getChannels().end())
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
