#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>
#include <iterator>

#include "../include/Channel.hpp"
#include "../include/Server.hpp"

class Channel;

class Client
{
    private:
        int _fd;
        std::string _nick;
        std::string _user;
        std::string _realname;
        std::string _pass;
        std::string _hostname;
        std::string _cmd;
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
        
        void setNickname(const std::string& nick);
        void setUser(const std::string& user);
        void setRealname(const std::string& realname);
        void setHasPass(const bool& has_pass);
        void setAuthenticated(const bool& autheticated);
        void setRegistered(const bool& registered);
        void setHostname(const std::string& hostname);
        void setCliCmd(std::string cmd);
        
        std::string getNickname() const;
        std::string getUser() const;
        std::string getRealname() const;
        std::string getHostname() const;
        std::string getCliCmd() const;
        std::string getNickWithPrefix(Channel& ch) const;
        bool getAuthenticated() const;
        bool getHasPass() const;
        bool getRegistered() const;
        int getFd() const;
};

#endif
