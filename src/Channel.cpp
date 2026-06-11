#include "../include/Channel.hpp"
#include "../include/Client.hpp"

Channel::Channel()
{}

Channel::Channel(std::string name, std::string topic, std::string modes, std::string key, int user_fd, Server *server)
{
    _name = name;
    _topic = topic;
    _modes = modes;
    _key = key;
    _user_fd = user_fd;
	_server = server;
    _limit = 0;
	if(_members_fd.empty())
		_members_fd.push_back(user_fd);
}

Channel::Channel(const Channel& copy) :
	_name(copy._name),
	_topic(copy._topic),
	_modes(copy._modes),
	_key(copy._key),
	_user_fd(copy._user_fd),
	_server(copy._server),
	_limit(copy._limit)
{}

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other)
    {
        _name = other._name;
        _topic = other._topic;
        _modes = other._modes;
        _user_fd = other._user_fd;
		_key = other._key;
		_limit= other._limit;
		_server = other._server;
    }
    return (*this);
}

Channel::~Channel()
{
}

std::string Channel::getChannelName() const
{
    return(_name);
}

void Channel::setChannelName(const std::string name)
{
    _name = name;
}

std::string Channel::getChannelTopic() const
{
    return(_topic);
}

void Channel::setChannelTopic(const std::string topic)
{
    _topic = topic;
}

std::string Channel::getChannelModes() const
{
    return(_modes);
}

std::string Channel::getChannelModeArgs()
{
	if (isModeEnabled('l'))
	{
		std::ostringstream oss;
		oss << _limit;
		return (oss.str());
	}
	return ("");
}

void Channel::setChannelMode(const char modechar)
{
	if (!isModeEnabled(modechar))
		_modes.append(1, modechar);
}

void Channel::unsetChannelMode(const char modechar)
{
	std::size_t pos = _modes.find(modechar);
	if (pos != std::string::npos)
		_modes.erase(pos, 1);
}

bool Channel::isModeEnabled(const char modechar)
{
	std::size_t pos = _modes.find(modechar);
	if (pos != std::string::npos)
		return true;
	return false;
}

int Channel::getLimit()
{
	return _limit;
}

void Channel::setLimit(int n)
{
	_limit = n;
}

std::vector<int> Channel::getChannelAdmins()
{
    return(_admind_fd);
}

std::vector<int> Channel::getClientsArray()
{
    return(_members_fd);
}

std::string& Channel::getChannelKey()
{
    return(_key);
}

void Channel::setKey(std::string key)
{
	_key = key;
}

void Channel::addMember(int fd)
{
    for (size_t i = 0; i < _members_fd.size(); i++)
        if (_members_fd[i] == fd)
            return;
    _members_fd.push_back(fd);
}

void Channel::addAdmind(int fd)
{
    for(size_t i = 0; i < _admind_fd.size(); i++)
        if(_admind_fd[i] == fd)
            return;
    _admind_fd.push_back(fd);
}

void Channel::addInvited(int fd)
{
    for(size_t i = 0; i < _invited_fd.size(); i++)
        if(_invited_fd[i] == fd)
            return;
    _invited_fd.push_back(fd);
}


void Channel::removeClient(int fd)
{
    if(isAdmin(fd))
    {
        
        for(std::vector<int>::iterator it =  _admind_fd.begin(); it != _admind_fd.end(); ++it)
        {
            if(*it == fd)
            {
                _admind_fd.erase(it);
                break ;
            }
        }
    }
    for (std::vector<int>::iterator it = _members_fd.begin(); it != _members_fd.end(); ++it)
    {
        if (*it == fd)
        {
            _members_fd.erase(it);
            return;
        }
    }
}

bool Channel::hasClient(const Client &client)
{
    std::vector<int>::iterator it;

    for (it = _members_fd.begin() ; it != _members_fd.end(); ++it)
    {
        if (*it == client.getFd())
            return true;
    }
    return false;
}

bool Channel::isAdmin(int fd)
{
    for(std::vector<int>::iterator it = _admind_fd.begin(); it != _admind_fd.end(); it++)
        if((*it) == fd)
            return true;
    return false;
}

bool Channel::isInvited(int fd)
{
    for(std::vector<int>::iterator it = _invited_fd.begin(); it != _invited_fd.end(); it++)
        if((*it) == fd)
            return true;
    return false;   
}

void Channel::sendMembers(std::string msg, int exclude)
{
	for (size_t i = 0; i < _members_fd.size(); ++i)
	{
		if (exclude && _members_fd[i] == exclude)
			continue;
		if (_server->getClients().count(_members_fd[i]) > 0)
		{
			Client &dest = _server->getClients()[_members_fd[i]];
			dest.sendMsg(msg);
		}
	}
}
