#include "Server.hpp"

void    commandKick(Server &s, Client &client, std::string line)
{
    std::istringstream iss(line);
    std::string channelName;
    std::string nick;
    std::string resto;
    iss >> channelName >> nick;
    std::getline(iss, resto);

    if (!s.getChannels().count(channelName))
	{
		// ERR_NOSUCHCHANNEL (403) "<client> <channel> :No such channel"
		std::string message = ":ircserver 403 " + client.getNickname() + " " + channelName + " :No such channel";
		print_message(client.getFd(), message);
		return;
	}
    if (!s.getChannels()[channelName]->isAdmin(client.getFd()))
    {
        std::cout << "El cliente no es un operador del canal" << std::endl;
        return;
    }
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
        if (!resto.empty() && resto[0] == ' ')
            resto.erase(0, 1);
        if (!resto.empty() && resto[0] == ':')
            resto.erase(0, 1);
        while (!resto.empty() && resto[0] == ' ')
            resto.erase(0, 1);
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
