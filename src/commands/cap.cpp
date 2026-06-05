#include "Client.hpp"

void commandCap(std::istringstream &iss, Client &client)
{
    std::string subcommand;
    if (!(iss >> subcommand))
    {
        print_message(client.getFd(), ":my_serv_irc 461 * CAP :Not enough parameters");
        return;
    }
    ft_toupper(subcommand);
    if (subcommand == "LS")
    {
        print_message(client.getFd(), ":my_serv_irc CAP * LS :");
    }
    else if (subcommand == "REQ")
    {
        std::string resto;
        std::getline(iss, resto);
        print_message(client.getFd(), ":my_serv_irc CAP * NAK " +  resto);
    }
    else if (subcommand == "END")
    {
        /*if (!client.getHasPass())
        {
            print_message(client.getFd(), ":my_serv_irc 464 * :Password required");
            return;
        }*/
        std::cout << "[SERVER] Negociación CAP finalizada." << std::endl;
        client.setAuthenticated(true);
    }
}
