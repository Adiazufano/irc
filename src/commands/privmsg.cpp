#include "Server.hpp"

void privmsg(Server &s, Client &c, std::string &line)
{
	(void)s;
	(void)c;

	std::istringstream iss1(line);
	std::string text;
	std::string targets_full;
	std::vector<std::string> targets;

	getline(iss1, targets_full, ' ');
	getline(iss1 >> std::ws, text);
	std::istringstream iss2(targets_full);
	for (std::string t; getline(iss2, t, ',');)
		targets.push_back(t);

}

/* #include <string>
#include <iostream>
#include <sstream>
#include <vector>

int main()
{
	std::string line("manolo,gonzalo      mira que  bonita esta la luna");

	std::istringstream iss1(line);
	std::string targets;
	std::vector<std::string> targets_v;
	std::string text;

	getline(iss1, targets, ' ');
	getline(iss1 >> std::ws, text);
	std::istringstream iss2(targets);
	for (std::string target; getline(iss2, target, ',');)
		targets_v.push_back(target);

	std::cout << "targets: >" << targets << "<" << std::endl;
	std::cout << "text:    >" << text << "<" << std::endl;
	std::vector<std::string>::iterator it;
	for (it = targets_v.begin(); it != targets_v.end(); it++)
		std::cout << *it << std::endl;

	return 0;
}

int main2()
{
	std::string line("manolo,gonzalo      mira que  bonita esta la luna");

	std::istringstream input;
	std::istringstream input2;
	std::string targets;
	std::string text;
	std::vector<std::string> targets_v;

	input.str(line);
	getline(input, targets, ' ');
	input >> std::ws;
	getline(input, text);

	std::cout << "targets: >" << targets << "<" << std::endl;
	std::cout << "text:    >" << text << "<" << std::endl;


	input2.str(targets);
	for (std::string target; getline(input2, target, ',');)
	{
		targets_v.push_back(target);
	}

	std::vector<std::string>::iterator it;
	for (it = targets_v.begin(); it != targets_v.end(); it++)
		std::cout << *it << std::endl;

	return 0;
} */
