#include "../include/Client.hpp"

Client::Client() : _fd(-1), _authenticated(false), _has_pass(false), _registered(false){}

Client::Client(int fd, std::string hostname) : _fd(fd), _hostname(hostname), _authenticated(false), _has_pass(false), _registered(false) {}

Client::Client(const Client& other) : _fd(other._fd), _user(other._user), _realname(other._realname), _pass(other._pass), _hostname(other._hostname), _authenticated(other._authenticated), _has_pass(other._has_pass), _registered(other._registered) {}

Client& Client::operator=(const Client& other)
{
    if (this != &other)
    {
        this -> _fd = other._fd;
        this -> _authenticated = other._authenticated;
        this -> _nick = other._nick;
        this -> _user = other._user;
        this -> _realname = other._realname;
        this -> _has_pass = other._has_pass;
        this -> _registered = other._registered;
        this -> _hostname = other._hostname;
    }
    return (*this);
}

Client::~Client() {}

int Client::getFd() const
{
    return _fd;
}

void Client::setNickname(const std::string& nickname)
{
    this -> _nick = nickname;
}

std::string Client::getNickname() const
{
    return _nick;
}

void Client::setUser(const std::string& user)
{
    this -> _user = user;
}

std::string Client::getUser() const
{
    return _user;
}

void Client::setRealname(const std::string& realname)
{
    this -> _realname = realname;
}

std::string Client::getRealname() const
{
    return _realname;
}

void Client::setAuthenticated(const bool& authenticated)
{
    this -> _authenticated = authenticated;
}

bool Client::getAuthenticated() const
{
    return _authenticated;
}

void Client::setHasPass(const bool& has_pass)
{
    this -> _has_pass = has_pass;
}

bool Client::getHasPass() const
{
    return _has_pass;
}

void Client::setRegistered(const bool& registered)
{
    _registered = registered;
}

bool Client::getRegistered() const
{
    return (_registered);
}

void Client::setHostname(const std::string& hostname)
{
    _hostname = hostname;
}

std::string Client::getHostname() const
{
     return _hostname;
}

