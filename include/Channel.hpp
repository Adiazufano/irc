#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include <iostream>
#include <vector>
#include <iterator>
#include <map>
#include <sstream>

#include "Client.hpp"
#include "Server.hpp"

class Server;

class Client;

class Channel
{
    private:
        std::string _name;
        std::string _topic;
        std::string _mode;
        std::string _key;
        int         _user_fd;  // En lugar de guardar el cliente guardaremos el fd y lo buscaremos después. Ya que los contenedores pueden mover su memoria al crecer y generar SEGV
        std::vector<int> _admind_fd;
        std::vector<int> _members_fd;
    
    public:
        Channel();
        Channel(std::string name, std::string topic, std::string mode, std::string key, int admin_fd);
        Channel(const Channel& copy);
        Channel& operator=(const Channel& other);
        ~Channel();

        std::string getChannelName() const;
        void setChannelName(const std::string name);
        std::string getChannelTopic() const;
        void setChannelTopic(const std::string topic);
        std::string getChannelMode() const;
        void setChannelMode(const std::string mode);
        std::vector<int> getClientsArray();

        void addClient(int fd);
        void addAdmind(int fd);
        void removeClient(int fd);
        bool hasClient(const Client &client);
        bool isAdmin(int fd);
        void removeClient(const Client &client);
        std::string& getChannelKey();

};

void joinChannel(Server &s, Client& client, std::string line);
void channelTopic(Server &s, Client &client, std::string line);

#endif
