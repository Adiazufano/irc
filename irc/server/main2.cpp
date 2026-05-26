#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <vector>

#define PORT "6669"

int	main()
{
	struct addrinfo hints;
	struct addrinfo *res;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;		// localhost

	getaddrinfo(0, PORT, &hints, &res);
	
	int serv_socket;
	serv_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	/* `SO_REUSEADDR` is missing
	Without `setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))`
	before `bind`, restarting your server quickly during testing will give you
	"address already in use" errors. */

	bind(serv_socket, res->ai_addr, res->ai_addrlen);

	/* Structural issue: `accept` is inside the loop
	This is the main problem. Right now you're blocking on `accept` at the top of
	every loop iteration, which means you must get a new connection before you 
	can read from existing clients. The typical pattern is the opposite: use `poll`
	to tell you which fd is ready, then decide whether it's the server socket
	(new connection) or a client socket (incoming data).
	The server socket itself should be in your `pollfd` array. When `poll` reports
	it as readable, that's when you call `accept`. */

	/* From `poll`'s perspective, the server socket is just another fd in the
	array — it has no idea which one is the "special" listener. The distinction
	only matters in *your* code, when you decide what to do after `poll` wakes up.

	The logic becomes:
	- `POLLIN` on the server socket			→ a new connection is knocking, call `accept`
	- `POLLIN` on a client socket			→ data is waiting, call `recv`
	- `POLLHUP` / `POLLERR` on any socket	→ something went wrong, close and remove it from the array

	So the typical structure is:
		poll(...)

		for each fd in the array:
		    if it has POLLIN:
		        if it's the server socket → accept
		        else → recv

	The only practical differences between the server socket and client sockets are:
	1. You add it to the array once, before the loop, and never remove it
	2. You check its fd against `serv_socket` to know which branch to take */

	std::vector<struct pollfd> arr;
	listen(serv_socket, 10);
	std::cout << "Server listening on port " << PORT << '\n';
	while (true)
	{
		int new_client;
		new_client = accept(serv_socket, 0, 0);
		struct pollfd pfd = {new_client, POLLIN, 0};
		arr.push_back(pfd);

		int poll_result = poll(arr.data(), arr.size(), -1);
		std::cout << "poll_result: " << poll_result << '\n';

		while (poll_result > 0)
		{
			std::vector<struct pollfd>::iterator it = arr.begin();
			while (it != arr.end())
			{
				if (it->revents != 0)
				{
					char buffer[512] = { 0 };
					recv(it->fd, buffer, sizeof(buffer), 0);
					std::cout << "Message from client: " << buffer << '\n';
					break;
				}
				it++;
			}
			poll_result--;
		}
	}

	close(serv_socket);
	std::cout << "Closed" << '\n';

	freeaddrinfo(res);

	return (0);
}
