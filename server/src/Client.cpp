#include "../include/Client.hpp"

Client::Client() : _fd(-1), _authenticated(false), _has_pass(false), _registered(false){}

Client::Client(int fd) : _fd(fd), _authenticated(false), _has_pass(false), _registered(false){}

Client::Client(const Client& other) : _fd(other._fd), _authenticated(other._authenticated), _has_pass(other._has_pass), _registered(other._registered){}

Client& Client::operator=(const Client& other)
{
    if (this != &other)
    {
        this -> _fd = other._fd;
        this -> _authenticated = false;
        this -> buffer = other.buffer;
        this -> _nick = other._nick;
        this -> _user = other._user;
        this -> _realname = other._realname;
        this -> _has_pass = other._has_pass;
        this -> _registered = other._registered;
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
