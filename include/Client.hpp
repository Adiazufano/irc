#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>
#include <iterator>

#include "../include/Channel.hpp"
#include "../include/Server.hpp"

class Client
{
    private:
        int _fd;
        std::string _nick;
        std::string _user;
        std::string _realname;
        std::string _pass;
        std::string _hostname;
        bool _authenticated;
        bool _has_pass;
        bool _registered;
        
        public:
        Client();
        Client(int fd, std::string hostname);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();
        std::string buffer; 
        int getFd() const;
        void setNickname(const std::string& nick);
        std::string getNickname() const;
        void setUser(const std::string& user);
        std::string getUser() const;
        void setRealname(const std::string& realname);
        std::string getRealname() const;
        void setAuthenticated(const bool& autheticated);
        bool getAuthenticated() const;
        void setHasPass(const bool& has_pass);
        bool getHasPass() const;
        void setRegistered(const bool& registered);
        bool getRegistered() const;
        std::string getHostname() const;
        void setHostname(const std::string& hostname);
};

#endif
