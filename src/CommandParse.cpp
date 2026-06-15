#include <sstream>
#include "../include/Client.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include "../include/Server.hpp"
#include "commands.hpp"

/*void print_message(int fd_client, const std::string& message)
{
    std::string complete_message = message + "\r\n";

    ssize_t n_bytes = send(fd_client, complete_message.c_str(), complete_message.size(), 0);
    if (n_bytes == -1)
        std::cerr << "Error al enviar datos al socket " << fd_client << std::endl;
}*/

void ft_toupper(std::string &str)
{
    for (unsigned long  i = 0; i < str.length(); i++)
        str[i] = toupper(str[i]);
}

int command_level(std::string cmd)
{
	if(cmd == "KICK")
		return(1);
	else if(cmd == "INVITE")
		return(2);
	else if(cmd == "TOPIC")
		return(3);
	else if(cmd == "MODE")
		return(4);
	else if(cmd == "JOIN")
		return(5);
	else if(cmd == "PRIVMSG")
		return(6);
	else if(cmd == "PART")
		return(7);
	else if(cmd == "PING")
		return(8);
	else if (cmd == "NICK")
		return(9);
	else if(cmd == "WHO")
		return(10);
	else if (cmd == "USER")
		return (11);
	else if (cmd == "PASS")
		return (12);
	else if (cmd == "QUIT")
		return (13);
	else if(cmd == "NOTICE")
		return(14);
	else if(cmd == "LIST")
		return(15);
	return(0);
}

void validate_command(Server &s, const std::string& cmd, Client &client)
{
    if (cmd.empty())
        return ;
    std::istringstream str(cmd);
    std::string command;
	std::string line;

	str >> command;
	client.setCliCmd(command);
	std::getline(str, line);
	if(!line.empty() && line[0] == ' ')
        line = line.substr(1);
	ft_toupper(command);
	int level = command_level(command); 
	
	if(command.empty())
		return ;
	std::istringstream iss(line);
	switch(level)
	{
		case 1:		commandKick(s, client, line);	break;
		case 2:		invite(s, client, line);		break;
		case 3:		channelTopic(s, client, line);	break;
		case 4:		mode(s, client, line);			break;
		case 5:		joinChannel(s, client, line);	break;
		case 6:		privmsg(s, client, line);		break;
		case 7:		partChannel(s, client, line);	break;
		case 8:		ping(client, line);				break;
		case 9:		commandNick(iss, client, s);	break;
		case 10:	whoCommand(s, client, line);	break;
		case 11: 	commandUser(iss, client);		break;
		case 12:	commandPass(iss, client, "");	break;
		case 13:	quit(s, client, line);			break;
		case 14:	notice(s, client, line);		break;
		case 15:	commandList(s, client, line);		break;
		default:
			client.sendMsg(ERR_UNKNOWNCOMMAND(client.getNickname(), client.getCliCmd()));
			break;
	}
}

void commandParse(const std::string& line, Client& client, std::string pass, Server &s)
{
    if (line.empty())
        return ;
    std::istringstream iss(line);
    std::string command;

    iss >> command;
    ft_toupper(command);
    if (command == "PASS" && client.getHasPass() != true)
        commandPass(iss, client, pass);
    else if (command == "PASS" && client.getHasPass() != false)
        std::cout << "Password already validated: " << std::endl;
    else if (command == "NICK")
        commandNick(iss, client, s);
    else if (command == "USER")
        commandUser(iss, client);
    else if (command == "CAP")
        commandCap(iss, client);
	
    else
        return;
    // --- BLOQUE DE VERIFICACIÓN (Añade esto al final de la función) ---
    std::cout << "\n=========================================\n";
    std::cout << " ESTADO DEL CLIENTE (Socket " << client.getFd() << "):\n";
    std::cout << "  - Nickname: [" << client.getNickname() << "]\n";
    std::cout << "  - Username: [" << client.getUser() << "]\n";
    std::cout << "  - Realname: [" << client.getRealname() << "]\n";
    std::cout << "=========================================\n\n";
}
