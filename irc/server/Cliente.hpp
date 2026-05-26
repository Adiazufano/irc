#ifndef CLIENTE_HPP
#define CLIENTE_HPP
#include <string>

class Cliente
{
    private:
        int _fd;
        std::string _nick;
        std::string _user;
        std::string _realname;
        bool _authenticated;
        bool _has_pass;

    public:
        std::string buffer;
        Cliente();
        Cliente(int fd);
        Cliente(const Cliente& other);
        Cliente& operator=(const Cliente& other);
        ~Cliente();
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
void commandParse(const std::string& line, Cliente& cliente);
void print_message(int fd_cliente, const std::string& message);
#endif
