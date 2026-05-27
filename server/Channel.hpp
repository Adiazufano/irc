#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"

#include <iostream>
#include <vector>
#include <iterator>

class Channel
{
    typedef std::vector<Client *>::iterator client_iterator;
    private:
        std::string _name;
        std::string _topic;
        std::string _mode;
        Client*     _admin;
        std::vector<Client *> _clients;
    
    public:
        Channel();
        Channel(std::string name, std::string topic, std::string mode, Client* _admin);
        Channel(const Channel& copy);
        Channel& operator=(const Channel& other);
        ~Channel();

        std::string getChannelName() const;
        void setChannelName(const std::string name);
        std::string getChannelTopic() const;
        void setChannelTopic(const std::string topic);
        std::string getChannelMode() const;
        void setChannelMode(const std::string mode);
        std::vector<Client *> getChannelClients() const;

        void addClient(Client * client);
        void removeClient(Client * client); 
};

void joinChannel(Client& client, std::string line, std::vector<Channel *> &channels);

#endif
