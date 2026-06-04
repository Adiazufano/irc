#include "Client.hpp"

void commandUser(std::istringstream &iss, Client& client)
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
        if (colon_pos == 0)
        {
            std::cout << "No se encontro <:>" << std::endl;
            return;
        }
        if (colon_pos  != std::string::npos)
            realname = resto.substr(colon_pos);
        else
            realname = resto;
    }
    
    if (username.empty() || realname.empty())
        return;
    
    client.setUser(username);
    client.setRealname(realname);
}
