#include "Server.hpp"

void    commandKick(Server &s, std::string line)
{
    std::istringstream iss(line);
    std::string channelName;
    std::string nick;
    std::string resto;
    iss >> channelName >> nick;
    std::getline(iss, resto);

    if (channelName.empty())
    {
       std::cout << "El canal no puede ser nulo" << std::endl;
       return ;
    }

    if (!checkName(channelName))
    {
        std::cout << "El nombre del canal no es correcto" << std::endl;
        return ;
    }

    std::map<std::string, Channel*>& channels = s.getChannels(); 
    if (channels.count(channelName) == 0)
    {
        std::cout << "Error: El canal " << channelName << " no existe" << std::endl;
        return;
    }

    std::map<std::string, int>& clients = s.getClientsByNick();
    std::map<std::string, int>::iterator it = clients.find(nick);
    if (it == clients.end())
    {
        std::cout << "Error: El cliente " << nick << " no existe en el servidor" << std::endl;
        return;
    }
	Channel *channel = channels[channelName];
	Client &cli = s.getClients()[s.getClientsByNick()[nick]];
    std::vector<int> _fd_clients = channel->getClientsArray();
    if (channel -> hasClient(cli))
    {
        channel -> removeClient(cli);
        if (resto.empty())
            std::cout << "El usuario " << nick << " ha sido expulsado de " << channelName << std::endl;
        else
            std::cout << "El usuario " << nick << " ha sido expulsado de " << channelName << " Por " << resto << std::endl;

    }
    else
    {
        std::cout << "Error: El usuario " << nick << " no esta en el canal" << std::endl;
    }
}
