#include "Client.hpp"

bool isValidChar(const std::string &line, const std::string &forbidden)
{
    for (size_t i = 0; i < line.length(); i++)
    {
        unsigned char c = line[i];
        if (c < 32 || c == 127) 
            return false;
    }
    if (!forbidden.empty() && line.find_first_of(forbidden) != std::string::npos)
        return false;
    return true;
}

void commandNick(std::istringstream &iss, Client &client, Server& s)
{
    std::string nickname;
    std::vector<std::string> chanels = client.getChannels();
    iss >> nickname;
    
    std::string oldNickname = client.getNickname().empty() ? "*" : client.getNickname();
    std::string nick_msg = NICK_MSG(oldNickname, client.getUser(), client.getHostname(), nickname);
    

    if (nickname.empty())
        return(client.sendMsg(ERR_NONICKNAMEGIVEN(oldNickname)));

    if (!isValidChar(nickname, " ,:*?!@#&"))
        return(client.sendMsg(ERR_ERRONEUSNICKNAME(oldNickname, nickname)));

    if (s.getClientsByNick().find(nickname) != s.getClientsByNick().end())
        return (client.sendMsg(ERR_NICKNAMEINUSE(oldNickname, nickname)));

    if (!client.getNickname().empty())
        s.getClientsByNick().erase(client.getNickname());

    client.setNickname(nickname);
    s.getClientsByNick()[nickname] = client.getFd();

    client.sendMsg(NICK_MSG(oldNickname, client.getUser(), client.getHostname(), nickname));

    std::vector<std::string>::iterator it;
    for (it = chanels.begin(); it != chanels.end(); ++it)
        s.getChannels()[*it] -> sendMembers(nick_msg);
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
