#include "Client.hpp"

void commandPass(std::istringstream &iss, Client &client, std::string pass)
{
    std::string password;
    if (iss >> password)
    {
        if (password == pass && !pass.empty())
        {
            client.setHasPass(true);
            std::cout << "[SERVER] Contraseña correcta para el socket " << client.getFd() << "\n";
        }
        else
        {
            print_message(client.getFd(), ":my_serv_irc 464 * :Password incorrect.");
            std::cout << "[SERVER] Contraseña INCORRECTA en el socket " << client.getFd() << "\n";
        }
    }
    else
    {
        print_message(client.getFd(), ":my_serv_irc 461 * PASS :Not enough parameters");
    }
}
