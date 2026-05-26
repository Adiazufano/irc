#include "Cliente.hpp"

Cliente::Cliente() : _fd(-1), _authenticated(false), _has_pass(false){}

Cliente::Cliente(int fd) : _fd(fd), _authenticated(false), _has_pass(false){}

Cliente::Cliente(const Cliente& other) : _fd(other._fd), _authenticated(other._authenticated), _has_pass(other._has_pass){}

Cliente& Cliente::operator=(const Cliente& other)
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
    }
    return (*this);
}

Cliente::~Cliente() {}

int Cliente::getFd() const
{
    return _fd;
}

void Cliente::setNickname(const std::string& nickname)
{
    this -> _nick = nickname;
}

std::string Cliente::getNickname() const
{
    return _nick;
}

void Cliente::setUser(const std::string& user)
{
    this -> _user = user;
}

std::string Cliente::getUser() const
{
    return _user;
}

void Cliente::setRealname(const std::string& realname)
{
    this -> _realname = realname;
}

std::string Cliente::getRealname() const
{
    return _realname;
}

void Cliente::setAuthenticated(const bool& authenticated)
{
    this -> _authenticated = authenticated;
}

bool Cliente::getAuthenticated() const
{
    return _authenticated;
}

void Cliente::setHasPass(const bool& has_pass)
{
    this -> _has_pass = has_pass;
}

bool Cliente::getHasPass() const
{
    return _has_pass;
}

