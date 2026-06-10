#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string.h>

#define my_serv_name ":my_serv_irc"


#define RPL_NOTOPIC(nick, alias)        (my_serv_name" 331 " + (nick) + " " + (alias) + " :No topic is set")
#define RPL_TOPIC(nick, alias, topic)   (my_serv_name" 332 " + (nick) + " " + (alias) + " :" + (topic))
#define RPL_NAMREPLY(nick, alias)       (my_serv_name" 353 " + (nick) + " = " + (alias) + " :")
#define RPL_ENDOFNAMES(nick, alias)     (my_serv_name" 366 " + (nick) + " " + (alias) + " :End of /NAMES list")


#define ERR_NEEDMOREPARAMS(nick, cmd)   (my_serv_name" 461 " + (nick) + " " + (cmd) + " :Not enough parameters")
#define ERR_BADCHANNELKEY(nick, chName) (my_serv_name" 475 " + (nick) + " " + (chName) + " :Cannot join channel")
#define ERR_BADCHANMASK(nick, chName)   (my_serv_name" 476" + (nick) + " " + (chName) + " :Bad Channel Mask")
#define ERR_NOSUCHCHANNEL(nick, chname) (my_serv_name" 403 " + (nick) + " " + (chname) + " :No such channel")
#define ERR_NOTONCHANNEL(nick, chname)  (my_serv_name" 442 " + (nick) + " " + (chname) + " :You're not on that channel")
#define ERR_CHANOPRIVSNEEDED(nick, chname) (my_serv_name" 482 " + (nick) + " " + (chname) + " :You're not channel operator")
#define ERR_USERNOTINCHANNEL(nick, chname) (my_serv_name" 441 " + (nick) + " " + (chname) + " :They aren't on that channel")

#define ERR_NOSUCHNICK(clientNick, nick)                    (my_serv_name" 401 " + (clientNick) + " " + (nick) + " :No such nick/channel")
#define ERR_CANNOTSENDTOCHAN(nick, channel)                 (my_serv_name" 404 " + (nick) + " " + (channel) + " :Cannot send to channel")
#define ERR_USERONCHANNEL(clientNick, nick, channel)        (my_serv_name" 443 " + (clientNick) + " " + (nick) + " " + (channel) + " :is already on channel")
#define RPL_INVITING(client, nick, channel)                 (my_serv_name" 341 " + (client) + " " + (nick) + " " + (channel))
#define INVITE_MSG(source, client, host, nick, channel)     (":" + (source) + "!" + (client) + "@" + (host) + " INVITE " + (nick) + " :" + (channel))
#define ERR_NORECIPIENT(client, command)                    (my_serv_name" 411 " + (client) + " :No recipient given (" + (command) + ")")
#define ERR_NOTEXTTOSEND(client)                            (my_serv_name" 412 " + (client) + " :No text to send")
#define ERR_UNKNOWNCOMMAND(client, command)                 (my_serv_name" 421 " + (client) + " " + (command) + " :Unknown command")
#define ERR_PASSWDMISMATCH(client)                          (my_serv_name" 464 " + (client) + " :Password incorrect")
#define ERR_NICKNAMEINUSE(client, nick)                     (my_serv_name" 433 " + (client) + " " + (nick) + " :Nickname is already in use")
#define ERR_NICKNAMEINUSE2(nick)                            (my_serv_name" 433 * " + (nick) + " :Nickname is already in use")

#endif
