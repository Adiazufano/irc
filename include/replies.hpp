#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string.h>

#define my_serv_name ":my_serv_irc"


#define RPL_NOTOPIC(nick, alias)        (my_serv_name" 331 " + (nick) + " " + (alias) + " :No topic is set")
#define RPL_TOPIC(nick, alias, topic)   (my_serv_name" 332 " + (nick) + " " + (alias) + " :" + (topic))
#define RPL_NAMREPLY(nick, alias)       (my_serv_name" 353 " + (nick) + " = " + (alias) + " :")
#define RPL_ENDOFNAMES(nick, alias)     (my_serv_name" 366 " + (nick) + " " + (alias) + " :End of /NAMES list")


#define ERR_NEEDMOREPARAMS(nick)        (my_serv_name" 461 " + (nick) + " JOIN :Not enough parameters")
#define ERR_BADCHANNELKEY(nick, chName) (my_serv_name" 475 " + (nick) + " " + (chName) + " :Cannot join channel")
#define ERR_BADCHANMASK(nick, chName)   (my_serv_name" 476" + (nick) + " " + (chName) + " :Bad Channel Mask")

#endif
