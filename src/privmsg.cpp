#include "../include/Server.hpp"

void privmsg(Server &s, Client &c, std::string &line)
{
	std::cout << "PRIVMSG\n";
	std::cout << "line =    '" << line << "'\n";
	(void)s;
	(void)c;
	size_t space_pos = line.find(' ');
	std::string targets = line.substr(0, space_pos);
	std::cout << "targets = '" << targets << "'\n";
	std::string text = line.substr(space_pos + 1);
	std::cout << "text =    '" << text << "'\n";

	std::vector<std::string> targets_v;
	size_t start = space_pos + 1;
	size_t found = targets.find(',');
	while (found)
	{
		targets_v.push_back(line.substr(start, found));
		start = found + 1;
		found = targets.find(',');
	}

	std::vector<std::string>::iterator it;
	for (it = targets_v.begin(); it != targets_v.end(); it++)
		std::cout << *it << std::endl;

}
