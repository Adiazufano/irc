#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <string>
#include <iterator>

class Client
{
    private:
        int _fd;
        std::string _nick;
        std::string _user;
        std::string _realname;
        std::string _pass;
        bool _authenticated;
        bool _has_pass;

    public:
        std::string buffer;
        Client();
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();
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
};
void commandParse(const std::string& line, Client& client, std::string pass);
void print_message(int fd_client, const std::string& message);
#endif
