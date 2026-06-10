#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "replies.hpp"


void namesCommand(Server&s, Channel& ch, Client& client)
{
    std::map<int, Client>& clientsMap = s.getClients();
    std::vector<int> clients = ch.getClientsArray();
    std::string nick = client.getNickname();
    std::string namesList;
    std::string name = ch.getChannelName();

    namesList = RPL_NAMREPLY(nick, name); 
    for(std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        namesList += clientsMap[*it].getNickWithPrefix(ch) + " ";
    }
    client.sendMsg(namesList); 
    client.sendMsg(RPL_ENDOFNAMES(nick, name));
}
