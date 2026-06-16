#include "Banner.hpp"
#include "Server.hpp"
#include "Debug.hpp"
#include <iostream>

#define BANNER 0

volatile sig_atomic_t g_server_running = 1;

void signalHandler (int signum) {
	(void)signum;
	g_server_running = 0;
}

int main (int argc, char **argv) {
	if (argc != 3) {
		std::cout << "./Usage: ./ircserv <port[0-65535]> <password>" << std::endl;
		return 1;
	}
	signal (SIGINT, signalHandler);
	signal (SIGQUIT, signalHandler);
	try {
		DEBUG ("main debug test");
		long portInput = std::atol (argv[1]);
		if (portInput < 0 || portInput > 65535) {
			std::cout << "Error: Invalid Port [" << portInput << "] detected." << std::endl;
			return 1;
		}
		int port = static_cast<int> (portInput);
		std::string password = argv[2];
		Server ircServer (port, password);
		if (BANNER) {
			printBannerTypewriter ();
		}
		ircServer.ircLoop ();
	} catch (const std::exception &e) {
		std::cout << "Error: " << e.what () << std::endl;
		return 1;
	}

	return 0;
}
