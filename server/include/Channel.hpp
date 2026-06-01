#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../include/Client.hpp"

#include <iostream>
#include <vector>
#include <iterator>

class Channel
{
    private:
        std::string _name;
        std::string _topic;
        std::string _mode;
        int         _user_fd;  // En lugar de guardar el cliente guardaremos el fd y lo buscaremos después. Ya que los contenedores pueden mover su memoria al crecer y generar SEGV
        std::vector<int> _admind_fd;
        std::vector<int> _clients_fd;
    
    public:
        Channel();
        Channel(std::string name, std::string topic, std::string mode, int admin_fd);
        Channel(const Channel& copy);
        Channel& operator=(const Channel& other);
        ~Channel();

        std::string getChannelName() const;
        void setChannelName(const std::string name);
        std::string getChannelTopic() const;
        void setChannelTopic(const std::string topic);
        std::string getChannelMode() const;
        void setChannelMode(const std::string mode);

        void addClient(int fd);
        void addAdmind(int fd);
        void removeClient(int fd);

};

void joinChannel(Client& client, std::string line, std::vector<Channel *> &channels);

#endif
