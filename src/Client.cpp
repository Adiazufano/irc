#include "../include/Client.hpp"

Client::Client() : _fd(-1), _authenticated(false), _has_pass(false), _registered(false){}

Client::Client(int fd, std::string hostname) : _fd(fd), _hostname(hostname), _authenticated(false), _has_pass(false), _registered(false) {}

Client::Client(const Client& other) : _fd(other._fd), _nick(other._nick), _user(other._user), _realname(other._realname), _pass(other._pass), _hostname(other._hostname), _cmd(other._cmd), _authenticated(other._authenticated), _has_pass(other._has_pass), _registered(other._registered) {}

Client& Client::operator=(const Client& other)
{
    if (this != &other)
    {
        this -> _fd = other._fd;
        this -> _authenticated = other._authenticated;
        this -> _nick = other._nick;
        this -> _user = other._user;
        this -> _cmd = other._cmd;
        this -> _channels = other._channels;
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

std::string Client::getCliCmd() const
{
    return(_cmd);
}

void Client::setCliCmd(std::string cmd)
{
    _cmd = cmd;
}

std::string Client::getNickWithPrefix(Channel& ch) const
{
    if(ch.isAdmin(_fd))
        return ("@" + _nick);
    return(_nick);
}


void Client::addChannel(Channel& ch)
{
    for (std::vector<std::string>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if (*it == ch.getChannelName())
            return;
    }
    _channels.push_back(ch.getChannelName());
}

void Client::removeChannel(Channel& ch)
{
    for(std::vector<std::string>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        if((*it) == ch.getChannelName())
        {
            _channels.erase(it);
            return ;
        }
}

void Client::sendMsg(std::string msg)
{
	// TO DO: Check if msg already contains "\r\n"

	if (msg.length() > 510)
		msg.erase(509);
	msg += "\r\n";
	ssize_t n_bytes = send(_fd, msg.c_str(), msg.size(), 0);
	if (n_bytes < 0)
		throw std::runtime_error(strerror(errno));
	else if (n_bytes < static_cast<ssize_t>(msg.size()))
	{
		// TO DO: Handle incomplete message
	}
}


void Client::printChannels()
{
   for(std::vector<std::string>::iterator it = _channels.begin(); it != _channels.end(); ++it)
   {
        std::cout << *it << std::endl;
   } 
}

void Client::printVectorInt(std::vector<int> &v)
{
   for(std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
   {
        std::cout << *it << " ";
   }
   std::cout << std::endl;
}


