#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"

#include <sstream>


void userMessages(Server &s, Client& client, std::string name)
{

    std::string topic = s.getChannels()[name]->getChannelTopic();
    std::vector<int> clients = s.getChannels()[name]->getClientsArray();
    std::map<int, Client>& clientsMap = s.getClients();
    std::string topicMsg;
    std::string namesList;
    std::string endNames;
    
    if(!topic.empty())      // Sólo se envía al cliente que se une al canal, no a todo el mundo.
    {
        topicMsg = ":my_serv_irc 332 " + client.getNickname() + " " + name + " :" + topic;
        print_message(client.getFd(), topicMsg);
    }
    else
        print_message(client.getFd(), ":my_serv_irc 331 " + client.getNickname() + " " + name + " :No topic is set");
    std::cout << topic << std::endl;
    // El cliente cuando se une tiene que recibir la lista de usuarios del canal.
    namesList = ":my_serv_irc 353 " + client.getNickname() + " = " + name + " :"; 
    for(std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        namesList += clientsMap[*it].getNickname() + " ";
    }
    std::cout << namesList << std::endl;
    print_message(client.getFd(), namesList);
    
    endNames = ":my_serv_irc 366 " + client.getNickname() + " " + name + " :End of /NAMES list";
    std::cout << endNames << std::endl;
    std::cout << "Dentro del canal" << std::endl;
    print_message(client.getFd(), endNames);
}



void joinMessages(Server &s, Client& client, std::string name)
{
    std::string joinMsg;
    std::vector<int> clients = s.getChannels()[name]->getClientsArray();

    // Formato mensaje IRC [ :origen CODIGO destino [parámetros] :texto final\r\n ]


    joinMsg = ":" + client.getNickname() + "!" + client.getUser() + "@" + client.getHostname()+ " JOIN " + name;
    for(std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it)   
        print_message(*it, joinMsg);

    userMessages(s, client, name);
}


bool checkName(std::string name)
{
    if(name[0] != '#' && name[0] != '&')      
        return false;
    return(true);
}

std::map<std::string, std::string> getChData(std::string names, std::string keys, Client& client)
{
    std::istringstream streamNames(names);
    std::istringstream streamKeys(keys);
    std::string chName;
    std::string chKeys;
    std::map<std::string, std::string> chData;
    std::string errorMsg;

    while(std::getline(streamNames, chName, ','))
    {
        if(!std::getline(streamKeys, chKeys, ','))
            chKeys = "";
        if(checkName(chName) == 0)
        {
            errorMsg = ":my_serv_irc 476" + client.getNickname() + " " + chName + " :Bad Channel Mask";
            print_message(client.getFd(), errorMsg);         
            return std::map<std::string, std::string> ();  // Devolvemos un vector vacío           
        }
        chData[chName] = chKeys;
    }

    if (std::getline(streamKeys, chKeys, ','))
    {
        errorMsg = ":my_serv_irc 476" + client.getNickname() + " " + " :Bad Channel Mask";
        print_message(client.getFd(), errorMsg);
        return std::map<std::string, std::string> ();
    }

    return (chData);

}

bool validKey(Channel* channel, std::string key)
{
    if(channel->getChannelKey().empty())
        return true;
    else if(channel->getChannelKey() == key)
        return true;
    else
        return false;
}


void joinChannel(Server &s, Client& client, std::string line)
{
    std::istringstream str(line);
    std::string _chName;
    std::string _chKey;
    std::map <std::string, std::string> _chData;
    std::string errorMsg;

    str >> _chName;
    str >> _chKey;
    _chData = getChData(_chName, _chKey, client);
    if(_chData.empty())
    {
        errorMsg = ":my_serv_irc 461 " + client.getNickname() + " JOIN :Not enough parameters";
        return;
    }

    for(std::map<std::string, std::string>::iterator it = _chData.begin(); it != _chData.end(); ++it)
    {
        std::string name = it->first;
        std::string key = it->second;

        if (s.getChannels().count(name) && validKey(s.getChannels()[name], key))
        {
            s.getChannels()[name]->addClient(client.getFd());
            joinMessages(s, client, name);
        }
        else if (s.getChannels().count(name) && !validKey(s.getChannels()[name], key))
        {
            errorMsg = ":my_serv_irc 475 " + client.getNickname() + " " + name + " :Cannot join channel";
            print_message(client.getFd(), errorMsg);
        }
        else
        {
            Channel* ch = new Channel (name, "", "", key, client.getFd());
            ch->addAdmind(client.getFd());
            s.getChannels()[name] = ch;
            joinMessages(s, client, name);
        }
    }
}

