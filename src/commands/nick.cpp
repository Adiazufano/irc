#include "Client.hpp"

void commandNick(std::istringstream &iss, Client &client, Server& s)
{
    std::string nickname;
    iss >> nickname;

    std::string oldNickname = client.getNickname().empty() ? "*" : client.getNickname();

    if (nickname.empty())
        return(client.sendMsg(ERR_NONICKNAMEGIVEN(oldNickname)));

    if ((nickname[0] == '#' || nickname[0] == ':'))
        return(client.sendMsg(ERR_ERRONEUSNICKNAME(oldNickname, nickname)));

    if (s.getClientsByNick()[nickname])
        return (client.sendMsg(ERR_NICKNAMEINUSE(oldNickname, nickname)));

    client.setNickname(nickname);
    s.getClientsByNick()[nickname] = client.getFd();

    // TO DO: Buscar respuesta
    /* 
    The NICK message may be sent from the server to clients to acknowledge their NICK command was successful,
    and to inform other clients about the change of nickname. In these cases, the <source> of the message will
    be the old nickname [ [ "!" user ] "@" host ] of the user who is changing their nickname.

    Message Examples:
      :WiZ NICK Kilroy          ; WiZ changed his nickname to Kilroy.
      :dan-!d@localhost NICK Mamoped
                                ; dan- changed his nickname to Mamoped.

    */
}
