#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <poll.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#include "Debug.hpp"
#include "ServerState.hpp"
#include "Parser.hpp"
#include "CommandDispatcher.hpp"
#include "CommandResult.hpp"

class Server {
private:
	int _port;
	std::string _password;

	std::vector<struct pollfd> _pollfds;
	int _serverFd;
	// int _maxClients;
	std::map<int, std::string> _recvBuffers;
	std::map<int, std::string> _sendBuffers;
	ServerState _state;

	void setupSocket ();
	void acceptNewClient ();
	void receiveData (int clientFd);
	void sendData (int clientFd);
	void disconnectClient (int clientFd);

	void processMessage (int clientFd, const std::string &message); // parse担当に渡すもの
	void queueResponse (int clientFd, const std::string &message);

public:
	Server (int port, std::string &password);
	~Server ();

	void ircLoop ();
};

#endif
