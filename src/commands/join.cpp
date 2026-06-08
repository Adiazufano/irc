#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "replies.hpp"

#include <sstream>


void userMessages(Server &s, Client& client, std::string name)
{

    std::string topic = s.getChannels()[name]->getChannelTopic();
    std::vector<int> clients = s.getChannels()[name]->getClientsArray();
    std::map<int, Client>& clientsMap = s.getClients();
    std::string nick = client.getNickname();
    std::string namesList;
    std::string endNames;
    
    if(!topic.empty())      // Sólo se envía al cliente que se une al canal, no a todo el mundo.
        print_message(client.getFd(), RPL_TOPIC(nick, name, topic));
    else
        print_message(client.getFd(), RPL_NOTOPIC(nick, name));
    std::cout << topic << std::endl;

    // El cliente cuando se une tiene que recibir la lista de usuarios del canal.
    namesList = RPL_NAMREPLY(nick, name); 
    for(std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        namesList += clientsMap[*it].getNickWithPrefix(*s.getChannels()[name]) + " ";
    }
    std::cout << namesList << std::endl;
    print_message(client.getFd(), namesList);
    
    std::cout << endNames << std::endl;
    std::cout << "Dentro del canal" << std::endl;
    print_message(client.getFd(), RPL_ENDOFNAMES(nick, name));
}



void joinMessages(Server &s, Client& client, std::string name)
{
    std::string joinMsg;
    std::vector<int> clients = s.getChannels()[name]->getClientsArray();

    // Formato mensaje IRC [ :origen CODIGO destino [parámetros] :texto final\r\n ]


    joinMsg = ":" + client.getNickname() + "!" + client.getUser() + "@" + client.getHostname()+ " " + client.getCliCmd() + " " + name;
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
    std::string nick = client.getNickname();
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
            print_message(client.getFd(), ERR_BADCHANMASK(nick, chName));         
            return std::map<std::string, std::string> ();  // Devolvemos un vector vacío           
        }
        chData[chName] = chKeys;
    }

    if (std::getline(streamKeys, chKeys, ','))
    {
        print_message(client.getFd(), ERR_BADCHANMASK(nick, chName));
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
    std::string nick = client.getNickname();
    std::string _chName;
    std::string _chKey;
    std::map <std::string, std::string> _chData;
    std::string errorMsg;

    str >> _chName;
    str >> _chKey;
    _chData = getChData(_chName, _chKey, client);
    if(_chData.empty())
    {
        errorMsg = ERR_NEEDMOREPARAMS(nick, client.getCliCmd());
        return;
    }

    for(std::map<std::string, std::string>::iterator it = _chData.begin(); it != _chData.end(); ++it)
    {
        std::string name = it->first;
        std::string key = it->second;

        if (s.getChannels().count(name) && validKey(s.getChannels()[name], key))
        {
            s.getChannels()[name]->addClient(client.getFd());
            client.addChannel(*(s.getChannels()[name]));
            joinMessages(s, client, name);
        }
        else if (s.getChannels().count(name) && !validKey(s.getChannels()[name], key))
        print_message(client.getFd(), ERR_BADCHANNELKEY(nick, name));
        else
        {
            Channel* ch = new Channel (name, "", "", key, client.getFd());
            ch->addAdmind(client.getFd());
            s.getChannels()[name] = ch;
            client.addChannel(*ch);
            joinMessages(s, client, name);
        }
    }
}
