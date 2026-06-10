#include "Client.hpp"

void commandNick(std::istringstream &iss, Client &client, Server& s)
{
    std::string nickname;
    iss >> nickname;
    if (nickname.empty())
        return;
    // s.getClientsByNick().insert(std::make_pair(nickname, client));
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
