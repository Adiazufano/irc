#include "Client.hpp"

void commandCap(std::istringstream &iss, Client &client)
{
    std::string subcommand;
    if (!(iss >> subcommand))
        return (client.sendMsg(ERR_NEEDMOREPARAMS(client.getNickname(), client.getCliCmd())));
    ft_toupper(subcommand);
    if (subcommand == "LS")
        client.sendMsg(":my_serv_irc CAP * LS :");
    else if (subcommand == "REQ")
    {
        std::string resto;
        std::getline(iss, resto);
        client.sendMsg(":my_serv_irc CAP * NAK " +  resto);
    }
    else if (subcommand == "END")
        client.setAuthenticated(true);
}
