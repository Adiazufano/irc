#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"


//TO DO consultar si cualquier miebro d eun canal puede cambiar el topic
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
        std::string reply = ":localhost 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
        print_message(client.getFd(), reply);
        return;
    }

    Channel &channel = (*it -> second);
    if (!channel.hasClient(client))
    {
        std::string reply = ":localhost 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n";
        print_message(client.getFd(), reply);
        return;
    }

    size_t index = 0;
    while (index < resto.length() && isspace(resto[index]))
        index++;
    if (index < resto.length() && resto[index] == ':')
        index++;
    topic = resto.substr(index);

    if (topic.empty() && resto.find(':') == std::string::npos)
    {
        if (channel.getChannelTopic().empty())
        {
            std::string reply = ":localhost 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
            print_message(client.getFd(), reply);
        }
        else
        {
            std::string reply = ":localhost 332 " + client.getNickname() + " " + channelName + " :" + channel.getChannelTopic() + "\r\n";
            print_message(client.getFd(), reply);
        }
    }
    else
    {
        channel.setChannelTopic(topic);
        std::string broadcast = ":" + client.getNickname() + "!" + client.getUser() + "@localhost" + " TOPIC " + channelName + " :" + topic + "\r\n";
        
        std::vector<int> clients = channel.getClientsArray();
        for (size_t i = 0; i < clients.size(); i++)
            print_message(clients[i], broadcast);
    }
}
