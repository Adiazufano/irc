#include "Client.hpp"

void cmdPass(std::istringstream &iss, Client &client, std::string pass)
{
    std::string password;

    if (client.getRegistered() && client.getAuthenticated())
        return(client.sendMsg(ERR_ALREADYREGISTERED(client.getNickname())));
    if (iss >> password)
    {
        if (password == pass && !pass.empty())
            client.setHasPass(true);
        else
            client.sendMsg(ERR_PASSWDMISMATCH(client.getNickname()));
    }
    else
        client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickname(), client.getCliCmd()));
}
