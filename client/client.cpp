// Coded by Claude.ai

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <csignal>
#include <poll.h>

#define PORT "6667"

static bool running = true;

void handle_sigint(int)
{
	running = false;
}

int main()
{
	signal(SIGINT, handle_sigint);

	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo("127.0.0.1", PORT, &hints, &res);

	int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	connect(sock, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);

	std::cout << "Connected to localhost:" << PORT << " (Ctrl+C to quit)";

	struct pollfd fds[2];
	fds[0] = {STDIN_FILENO, POLLIN, 0};  // keyboard input
	fds[1] = {sock,         POLLIN, 0};  // server messages

	while (running)
	{
		int ready = poll(fds, 2, 500);  // 500ms timeout so SIGINT is checked regularly

		if (ready < 0)
			break;

		// User typed something
		if (fds[0].revents & POLLIN)
		{
			std::string line;
			if (!std::getline(std::cin, line))
				break;
			line += "\r\n";
			send(sock, line.c_str(), line.size(), 0);
		}

		// Server sent something
		if (fds[1].revents & POLLIN)
		{
			char buffer[512] = {0};
			int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
			if (bytes <= 0)
			{
				std::cout << "Server disconnected\n";
				break;
			}
			std::cout << "Server: " << buffer;
		}
	}

	std::cout << "\nDisconnected\n";
	close(sock);
	return 0;
}
