#include "Client.hpp"

void commandNick(std::istringstream &iss, Client &client, Server& s)
{
    std::string nickname;
    iss >> nickname;
    if (nickname.empty() && s.getClientsByNick()[client.getNickname()])
        return(client.sendMsg(ERR_NONICKNAMEGIVEN(client.getNickname())));
    else if (nickname.empty() && !s.getClientsByNick()[client.getNickname()])
        return(client.sendMsg(ERR_NONICKNAMEGIVEN2()));
    // s.getClientsByNick().insert(std::make_pair(nickname, client));

    if ((nickname[0] == '#' || nickname[0] == ':') && s.getClientsByNick()[client.getNickname()])
        return(client.sendMsg(ERR_ERRONEUSNICKNAME(client.getNickname(), nickname)));
    else  if ((nickname[0] == '#' || nickname[0] == ':') && !s.getClientsByNick()[client.getNickname()])
        return(client.sendMsg(ERR_NONICKNAMEGIVEN(nickname)));

    if (s.getClientsByNick()[nickname])
    {
        if (s.getClientsByNick()[client.getNickname()])
            client.sendMsg(ERR_NICKNAMEINUSE(client.getNickname(), nickname));
        else
            client.sendMsg(ERR_NICKNAMEINUSE2(nickname));
        return ;
    }
    client.setNickname(nickname);
    s.getClientsByNick()[nickname] = client.getFd();
}
