#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

void quit(Server &s, Client &cli, std::string line)
{
    std::istringstream iss(line);
    std::vector<std::string>::iterator it;
    std::vector<std::string> channels = cli.getChannels();
    std::string reason = getMsg(iss);
    std::string quit_msg = QUIT_MSG(cli.getNickname(), cli.getUser(), cli.getHostname(), reason);
    Channel *ch = NULL;

    for (it = channels.begin(); it != channels.end(); ++it)
    {
        ch = s.getChannels()[*it];
        if (ch)
            ch->removeClient(cli.getFd());
        ch->sendMembers(quit_msg);
        cli.removeChannel(*ch);
    }
    //TODO: añadir si no hay  ningun cliente deberia eliminarse el canal
    s.getDisconnectedSockets().push_back(cli.getFd());
    cli.sendMsg(quit_msg);
}
