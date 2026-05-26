#include <sstream>
#include "Cliente.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

void print_message(int fd_cliente, const std::string& message)
{
    std::string complete_message = message + "\r\n";

    ssize_t n_bytes = send(fd_cliente, complete_message.c_str(), complete_message.size(), 0);
    if (n_bytes == -1)
        std::cerr << "Error al enviar datos al socket " << fd_cliente << std::endl;
}

void commandParse(const std::string& line, Cliente& cliente)
{
    if (line.empty())
        return ;
    std::istringstream iss(line);
    std::string command;

    iss >> command >> std::uppercase;

    if (command == "PASS")
    {
        std::string password;
        if (iss >> password)
        {
            if (password == "1234")
            {
                cliente.setHasPass(true);
                std::cout << "[SERVER] Contraseña correcta para el socket " << cliente.getFd() << "\n";
            }
            else
            {
                print_message(cliente.getFd(), ":my_serv_irc 464 * :Password incorrect.");
                std::cout << "[SERVER] Contraseña INCORRECTA en el socket " << cliente.getFd() << "\n";
            }
        }
    }
    else if (command == "NICK" && cliente.getHasPass() != false)
    {
        std::string nickname;
        iss >> nickname;
        if (nickname.empty())
            return;
        cliente.setNickname(nickname);
    }
    else if (command == "USER" && cliente.getHasPass() != false)
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
            size_t colon_pos = resto.find(':');
            if (colon_pos != std::string::npos)
                realname = resto.substr(colon_pos + 1);
            else
                realname = resto;
        }

        if (username.empty() || realname.empty())
            return;

        cliente.setUser(username);
        cliente.setRealname(realname);
    }
    else
        return;
    // ... todo tu código de parseo que ya tienes ...

    // --- BLOQUE DE VERIFICACIÓN (Añade esto al final de la función) ---
    std::cout << "\n=========================================\n";
    std::cout << " ESTADO DEL CLIENTE (Socket " << cliente.getFd() << "):\n";
    std::cout << "  - Nickname: [" << cliente.getNickname() << "]\n";
    std::cout << "  - Username: [" << cliente.getUser() << "]\n"; // Usa el getter real de tu clase
    std::cout << "  - Realname: [" << cliente.getRealname() << "]\n";
    std::cout << "=========================================\n\n";
}
