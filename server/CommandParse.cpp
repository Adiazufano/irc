#include <sstream>
#include "Client.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include "Server.hpp"

void print_message(int fd_client, const std::string& message)
{
    std::string complete_message = message + "\r\n";

    ssize_t n_bytes = send(fd_client, complete_message.c_str(), complete_message.size(), 0);
    if (n_bytes == -1)
        std::cerr << "Error al enviar datos al socket " << fd_client << std::endl;
}

void commandParse(const std::string& line, Client& client, std::string pass)
{
    if (line.empty())
        return ;
    std::istringstream iss(line);
    std::string command;

    iss >> command;
    for(unsigned long i = 0; i < command.length(); i++)
        command[i] = std::toupper(command[i]);
    if (command == "PASS" && client.getHasPass() != true)
    {
        std::string password;
        if (iss >> password)
        {
            if (password == pass)
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
    }
    else if (command == "PASS" && client.getHasPass() != false)
        std::cout << "Password already validated: " << std::endl;
    else if (command == "NICK" && client.getHasPass() != false)
    {
        std::string nickname;
        iss >> nickname;
        if (nickname.empty())
            return;
        client.setNickname(nickname);
    }
    else if (command == "USER" && client.getHasPass() != false)
    {
        std::string username;
        std::string mode;
        std::string desuso;
        std::string realname;

        iss >> username >> mode >> desuso;
        std::string resto;
        std::getline(iss, resto);
        
        if (!resto.empty())
        {
            size_t colon_pos = 0;
            size_t index = 0;
            bool flag = 0;
            while (index < resto.length())
            {
                if (isspace(resto[index]) && flag == 0)
                    colon_pos++;
                else
                    flag = 1;
                index++;
            }
            colon_pos += resto.find(':');
            if (colon_pos  != std::string::npos)
                realname = resto.substr(colon_pos + 1);
            else
                realname = resto;
        }
        
        if (username.empty() || realname.empty())
            return;
        
        client.setUser(username);
        client.setRealname(realname);
    }
    else
        return;
    // ... todo tu código de parseo que ya tienes ...

    // --- BLOQUE DE VERIFICACIÓN (Añade esto al final de la función) ---
    std::cout << "\n=========================================\n";
    std::cout << " ESTADO DEL CLIENTE (Socket " << client.getFd() << "):\n";
    std::cout << "  - Nickname: [" << client.getNickname() << "]\n";
    std::cout << "  - Username: [" << client.getUser() << "]\n"; // Usa el getter real de tu clase
    std::cout << "  - Realname: [" << client.getRealname() << "]\n";
    std::cout << "=========================================\n\n";
}
