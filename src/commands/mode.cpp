#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

// MUST always have a parameter
void modeTypeB(Channel &channel, char modechar, char modeset, std::istringstream &args)
{
	(void)channel;
	(void)modeset;
	(void)args;
	std::string modearg;
	args >> modearg;
	if (modearg.empty())
	{
		// Missing argument
		// If a type B or C mode does not have a parameter when being set, the server MUST ignore that mode.
		return;
	}
	if (modechar == 'o' && modeset == '+')
	{
		//channel.addOperator(modearg);
	}
	else if (modechar == 'o' && modeset == '-')
	{
		//channel.removeOperator(modearg);
	}
	// ¡CUIDADO! Si nick no existe el servidor debe responder ERR_NOSUCHNICK (401).
	// Es decir, hará falta la referencia al cliente para mandarle el error
}

void appendModesetModechar(std::string &modestring, char modeset, char modechar)
{
	char active = 0;
	//for (size_t i = 0; i < modestring.size(); ++i)
	//{
	//	if (modestring[i] == '+' || modestring[i] == '-')
	//		active = modestring[i];
	//}

	size_t pos = modestring.find_last_of("+-");
	if (pos != std::string::npos)
		active = modestring[pos];

	if (modeset != active)
		modestring.append(1, modeset);
	modestring.append(1, modechar);
}

// MUST have a parameter when being set, and MUST NOT have a parameter when being unset
void modeTypeC(Channel &channel, char modechar, char modeset, std::istringstream &args, std::vector<std::string> &result)
{
	if (modeset == '-')
	{
		if (!channel.isModeEnabled(modechar))
			return;
		channel.unsetChannelMode(modechar);
	}
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
		if (modechar == 'l')
		{
			/* static int stoi( std::string & s ) {
			   int i;
			   std::istringstream(s) >> i;
			   return i;
			} */
			int value;
			std::istringstream iss(modearg);
			iss >> value;
			if (iss.fail() || !iss.eof())	// COMPROBAR ESTO !!!!!
				return;
			if (channel.getLimit() == value)
				return;
			channel.setChannelMode('l');
			channel.setLimit(value);
		}
		else if (modechar == 'k')
		{
			if (channel.getChannelKey() == modearg)
				return;
			channel.setChannelMode('k');
			channel.setKey(modearg);
		}
		result.push_back(modearg);
	}
	appendModesetModechar(result[0], modeset, modechar);
}

// MUST NOT have a parameter
void modeTypeD(Channel &channel, char modechar, char modeset, std::vector<std::string> &result)
{
	if (modeset == '-')
	{
		if (!channel.isModeEnabled(modechar))
			return;
		channel.unsetChannelMode(modechar);
	}
	else if (modeset == '+')
	{
		if (channel.isModeEnabled(modechar))
			return;
		channel.setChannelMode(modechar);
	}
	appendModesetModechar(result[0], modeset, modechar);
}

void mode(Server &s, Client &c, std::string &line)
{
	(void)c;
	//std::string line = "    #roma    +iol     alice     50";
	// Parse target and modestring
	std::istringstream	iss(line);
	std::string			target;
	std::string			modestring;

	iss >> target >> modestring;

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
		#define RPL_CHANNELMODEIS(client, channel, modestring, args) (my_serv_name" 324 " + (client) + " " + (channel) + " " + (modestring) + " " + (args))
		std::string msg = RPL_CHANNELMODEIS(c.getNickname(), channel.getChannelName(), channel.getChannelModes(), channel.getChannelModeArgs());
		c.sendMsg(msg);
	}

	// Check if client has operator privileges
	// If a user does not have appropriate privileges to change modes on the target channel,
	// the server MUST NOT process the message, and ERR_CHANOPRIVSNEEDED (482) numeric is returned.

	char modeset = 0;
	std::vector<std::string> result;
	result.push_back("");
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
			modeTypeB(channel, modestring[i], modeset, iss);
		else if (modestring[i] == 'k' || modestring[i] == 'l')
			modeTypeC(channel, modestring[i], modeset, iss, result);
		else if (modestring[i] == 'i' || modestring[i] == 't')
			modeTypeD(channel, modestring[i], modeset, result);
		else
		{
			// Unknown or unsupported mode
		}
	}

	// When the server is done processing the modes, a MODE command is sent to all members
	// of the channel containing the mode changes.
	// Servers MAY choose to hide sensitive information when sending the mode changes.

	std::cout << "Mode result: ";
	for (std::vector<std::string>::iterator str = result.begin(); str != result.end(); ++str)
	{
		std::cout << *str << " ";
	}
	//std::cout << "\n";
	//std::cout << "Active modes: " << channel.getChannelModes() << "\n";
	//std::cout << "Is i enabled?" << channel.isModeEnabled('i') << "\n";
	//std::cout << "Is l enabled?" << channel.isModeEnabled('l') << "\n";
	//std::cout << "Is t enabled?" << channel.isModeEnabled('t') << "\n";
	//std::cout << "Limit: " << channel.getLimit() << "\n";
	//std::cout << "Key: " << channel.getChannelKey() << "\n";
}
