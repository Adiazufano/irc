#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Client.hpp"
#include "Server.hpp"

class Client;
class Server;

void commandCap(std::istringstream &iss, Client &client);
void joinChannel(Server &s, Client& client, std::string line);
void commandNick(std::istringstream &iss, Client &client);
void commandPass(std::istringstream &iss, Client &client, std::string pass);
void channelTopic(Server &s, Client &client, std::string line);
void commandUser(std::istringstream &iss, Client& client);
void privmsg(Server &s, Client &c, std::string &line);

void validate_command(Server &s, const std::string& cmd, Client &client);
void commandParse(const std::string& line, Client& client, std::string pass);
void print_message(int fd_client, const std::string& message);

#endif
