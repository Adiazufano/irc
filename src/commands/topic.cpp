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
        return(client.sendMsg(ERR_NOSUCHCHANNEL(client.getNickname(), channelName)));

    Channel &channel = (*it -> second);
    if (!channel.hasClient(client))
        return (client.sendMsg(ERR_NOTONCHANNEL(client.getNickname(), channelName)));

    size_t index = 0;
    while (index < resto.length() && isspace(resto[index]))
        index++;
    if (index < resto.length() && resto[index] == ':')
        index++;
    topic = resto.substr(index);

    if (topic.empty() && resto.find(':') == std::string::npos)
    {
        if (channel.getChannelTopic().empty())
            client.sendMsg(RPL_NOTOPIC(client.getNickname(), channelName));
        else
            client.sendMsg(RPL_TOPIC(client.getNickname(), channelName, topic));
    }
    else
    {
        channel.setChannelTopic(topic);
        std::string broadcast = ":" + client.getNickname() + "!" + client.getUser() + "@localhost" + " TOPIC " + channelName + " :" + topic + "\r\n";
        channel.sendMembers(s, broadcast, 0);
    }
}
