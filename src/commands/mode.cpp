#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

// MUST always have a parameter
void modeTypeB(char modechar, char modeset, std::istringstream &args)
{
	std::string modearg;
	args >> modearg;
	if (modearg.empty())
	{
		// Missing argument
		// If a type B or C mode does not have a parameter when being set, the server MUST ignore that mode.
		return;
	}
	if (modechar == 'o' && modeset == '+')
		channel.addOperator(modearg);
	else if (modechar == 'o' && modeset == '-')
		channel.removeOperator(modearg);

	// ¡CUIDADO! Si nick no existe el servidor debe responder ERR_NOSUCHNICK (401).
	// Es decir, hará falta la referencia al cliente para mandarle el error
}

// MUST have a parameter when being set, and MUST NOT have a parameter when being unset
void modeTypeC(char modechar, char modeset, std::istringstream &args)
{
	if (modeset == '-')
		channel.unsetMode(modechar);
	else if (modeset == '+')
	{
		std::string modearg;
		args >> modearg;
		if (modearg.empty())
		{
			// Missing argument
			// If a type B or C mode does not have a parameter when being set, the server MUST ignore that mode.
			return;
		}
		// Check if modearg is a valid number for limit
		channel.setMode(modechar, modearg);
	}
}

// MUST NOT have a parameter
void modeTypeD(char modechar, char modeset)
{
	if (modeset == '-')
		channel.unsetMode(modechar);
	else if (modeset == '+')
		channel.setMode(modechar);
}

void mode(Server &s, Client &c, std::string &line)
{
	std::string line = "    #roma    +iol     alice     50";
	// Parse target, modestring and modearguments
	std::istringstream	iss(line);
	std::string			target;
	std::string			modestring;
	//std::string			modeargs;

	iss >> target >> modestring;
	//getline(iss >> std::ws, modeargs);
	std::cout << "Target     : " << target << '\n';
	std::cout << "Modestring : " << modestring << '\n';
	//std::cout << "Modeargs   : " << modeargs << '\n';

	// Check if target is valid channel name (#...)

	if (!s.getChannels().count(target))
	{
		// Channel not found
		// ERR_NOSUCHCHANNEL (403)
	}

	Channel &channel = *(s.getChannels()[target]);

	// If <modestring> is not given, the RPL_CHANNELMODEIS (324) numeric is returned.
	// Servers MAY choose to hide sensitive information such as channel keys when sending the current modes.
	// Servers SHOULD also return the RPL_CREATIONTIME (329) numeric following RPL_CHANNELMODEIS.
	if (modestring.empty())
	{
		// RPL_CHANNELMODEIS (324)
	}

	// Check if client has operator privileges
	// If a user does not have appropriate privileges to change modes on the target channel,
	// the server MUST NOT process the message, and ERR_CHANOPRIVSNEEDED (482) numeric is returned.

	char modeset = 0;
	for (size_t i = 0; i < modestring.length(); ++i)
	{
		if (std::string("iktol").find_first_of(modestring[i]) != std::string::npos && !modeset)
		{
			// modeset has not been set
			// send error?
			return;
		}
		if (modestring[i] == '+' || modestring[i] == '-')
			modeset = modestring[i];
		else if (modestring[i] == 'o')
			modeTypeB(modestring[i], modeset, iss);
		else if (modestring[i] == 'k' || modestring[i] == 'l')
			modeTypeC(modestring[i], modeset, iss);
		else if (modestring[i] == 'i' || modestring[i] == 't')
			modeTypeD(modestring[i], modeset);
		else
		{
			// Unknown or unsupported mode
		}
	}

	// When the server is done processing the modes, a MODE command is sent to all members
	// of the channel containing the mode changes.
	// Servers MAY choose to hide sensitive information when sending the mode changes.
}
