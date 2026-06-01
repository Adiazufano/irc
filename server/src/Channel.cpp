#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel()
{}

Channel::Channel(std::string name, std::string topic, std::string mode, int user_fd)
{
    _name = name;
    _topic = topic;
    _mode = mode;
    _user_fd = user_fd;
    if(_clients_fd.empty())
        _clients_fd.push_back(user_fd);

    // std::cout << "Te has unido al canal: Name " << _name << " Topic: " << _topic << " Mode: " << _mode << " user: " << _user_fd->getUser() << std::endl; 
}

Channel::Channel(const Channel& copy) : _name(copy._name), _topic(copy._topic), _mode(copy._mode), _user_fd(copy._user_fd){}

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other)
    {
        _name = other._name;
        _topic = other._topic;
        _mode = other._mode;
        _user_fd = other._user_fd;
    }
    return (*this);
}

Channel::~Channel(){}

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
std::string Channel::getChannelMode() const
{
    return(_mode);
}
void Channel::setChannelMode(const std::string mode)
{
    _mode = mode;
}




void Channel::addClient(int fd)
{
    for (size_t i = 0; i < _clients_fd.size(); i++)
        if (_clients_fd[i] == fd)
            return;
    _clients_fd.push_back(fd);
}

void Channel::addAdmind(int fd)
{
    for(size_t i = 0; i < _admind_fd.size(); i++)
        if(_admind_fd[i] == fd)
            return;
    _admind_fd.push_back(fd);
}


void Channel::removeClient(int fd)
{
    for (std::vector<int>::iterator it = _clients_fd.begin(); it != _clients_fd.end(); ++it)
    {
        if (*it == fd)
        {
            _clients_fd.erase(it);
            return;
        }
    }
}
