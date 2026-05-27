#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel()
{}

Channel::Channel(std::string name, std::string topic, std::string mode, Client* _admin)
{
    _name = name;
    _topic = topic;
    _mode = mode;
    if(_clients.empty())
        _clients.push_back(_admin);

}

Channel::Channel(const Channel& copy) : _name(copy._name), _topic(copy._topic), _mode(copy._mode), _admin(copy._admin){}

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other)
    {
        _name = other._name;
        _topic = other._topic;
        _mode = other._mode;
        _admin = other._admin;
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
std::vector<Client *> Channel::getChannelClients() const
{
    return(_clients);
}



void Channel::addClient(Client * client)
{
    
    for(std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if((*it)->getUser() == client->getUser())
            return ;
    }
    _clients.push_back(client);
}



void Channel::removeClient(Client * client)
{
    for(std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if((*it)->getUser() == client->getUser())
            _clients.erase(it);
    }
} 