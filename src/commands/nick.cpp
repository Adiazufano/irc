#include "Client.hpp"

void commandNick(std::istringstream &iss, Client &client, Server& s)
{
    std::string nickname;
    iss >> nickname;
    if (nickname.empty())
        return;
    client.setNickname(nickname);
	s.getClientsByNick()[nickname] = client.getFd();
    // s.getClientsByNick().insert(std::make_pair(nickname, client));
}
