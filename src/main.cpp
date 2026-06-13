#include "Banner.hpp"
#include "Server.hpp"
#include "Debug.hpp"
#include <iostream>

#define BANNER 0

volatile sig_atomic_t g_server_running = 1;

void signalHandler(int signum) {
	(void)signum;
	g_server_running = 0;
}

int main (int argc, char **argv) {
	if(argc != 3) {
		std::cout << "./Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	try {
		DEBUG("main debug test");
		int port = std::atoi(argv[1]);
		std::string password = argv[2];
		Server ircServer(port, password);
		if (BANNER) {
			printBannerTypewriter ();
		}
		ircServer.ircLoop();
	} catch (const std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
