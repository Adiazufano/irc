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
		Server		*_server;
    
    public:
        Channel();
        Channel(std::string name, std::string topic, std::string mode, std::string key, int admin_fd, Server *server);
        Channel(const Channel& copy);
        Channel& operator=(const Channel& other);
        ~Channel();

        std::vector<int> getClientsArray();
        std::vector<int> getChannelAdmins();
        std::string getChannelTopic() const;
        std::string getChannelName() const;
        std::string getChannelMode() const;
        std::string& getChannelKey();
        
        void setChannelName(const std::string name);
        void setChannelTopic(const std::string topic);
        void setChannelMode(const std::string mode);

        bool isAdmin(int fd);
        bool hasClient(const Client &client);
        void addMember(int fd);
        void addAdmind(int fd);
        void removeClient(int fd);

		void sendMembers(std::string &msg, int exclude = 0);

};

void joinChannel(Server &s, Client& client, std::string line);
void channelTopic(Server &s, Client &client, std::string line);
void partChannel(Server &s, Client& client, std::string line);

#endif
