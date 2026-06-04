#include "Client.hpp"

void commandNick(std::istringstream &iss, Client &client)
{
    std::string nickname;
    iss >> nickname;
    if (nickname.empty())
        return;
    client.setNickname(nickname);
}
