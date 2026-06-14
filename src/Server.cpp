#include "Server.hpp"

extern volatile sig_atomic_t g_server_running;

Server::Server (int port, std::string &password)
	: _port (port), _password (password), _serverFd (-1), _state (password) {
	DEBUG ("コンストラクタ");
	setupSocket ();
}

Server::~Server () {
	DEBUG ("デストラクタ");
	for (size_t i = 0; i < _pollfds.size (); ++i) {
		if (_pollfds[i].fd >= 0) {
			close (_pollfds[i].fd);
		}
	}
}

void Server::setupSocket () {
	/*
		ソケットの作成
		AF_INET : IPv4
		SOCK_STREAM : TCP
	*/ 
	_serverFd = socket (AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0) {
		throw std::runtime_error ("Failed to create socket");
	}

	/*
		サーバー連続立ち上げ
	*/
	int opt = 1;
	if (setsockopt (_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) < 0) {
		throw std::runtime_error ("Failed to set socket options");
	}

#ifdef __APPLE__
	if (fcntl (_serverFd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error ("Failed to set non-blocking mode");
	}
#endif
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons (_port);

	if (bind (_serverFd, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) < 0) {
		throw std::runtime_error ("Failed to bind socket");
	}

	if (listen (_serverFd, SOMAXCONN) < 0) {
		throw std::runtime_error ("Failed to listen on socket");
	}

	struct pollfd pfd;
	pfd.fd = _serverFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollfds.push_back (pfd);

	std::cout << "Server listening on port " << _port << std::endl;
}

void Server::acceptNewClient () {
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof (clientAddr);
	int clientFd = accept (_serverFd, (struct sockaddr *)&clientAddr, &clientLen);

	if (clientFd >= 0) {
#ifdef __APPLE__
		fcntl (clientFd, F_SETFL, O_NONBLOCK);
#endif
		struct pollfd pfd;
		pfd.fd = clientFd;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_pollfds.push_back (pfd);

		_recvBuffers[clientFd] = "";
		_sendBuffers[clientFd] = "";

		_state.addNewClient (clientFd);

		std::cout << "New client connected. FD: " << clientFd << std::endl;
	}
}

void Server::receiveData (int clientFd) {
	char buffer[512];

#ifdef __linux__
	ssize_t bytesRead = recv (clientFd, buffer, sizeof (buffer) - 1, MSG_DONTWAIT);
#elif defined(__APPLE__)
	ssize_t bytesRead = recv (clientFd, buffer, sizeof (buffer) - 1, 0);
#endif

	if (bytesRead < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
			return;
		}
		disconnectClient (clientFd);
		return;
	} else if (bytesRead == 0) {
		disconnectClient (clientFd);
		return;
	} else {
		_recvBuffers[clientFd].append (buffer, bytesRead);

		// バッファ内\n確認
		size_t pos;
		while ((pos = _recvBuffers[clientFd].find ("\n")) != std::string::npos) {
			std::string line = _recvBuffers[clientFd].substr (0, pos);

			// (\r\n)
			if (!line.empty () && line[line.length () - 1] == '\r') {
				line.erase (line.length () - 1);
			}
			_recvBuffers[clientFd].erase (0, pos + 1);

			processMessage (clientFd, line);
			if (_recvBuffers.find (clientFd) == _recvBuffers.end ()) {
				return;
			}
		}
	}
}

void Server::ircLoop () {
	while (g_server_running) {
		int res = poll (&_pollfds[0], _pollfds.size (), -1);

		if (res < 0) {
			if (errno == EINTR) {
				std::cout << "\nServer stopping..." << std::endl;
				break;
			}
			throw std::runtime_error ("Poll failed.");
		}
		for (size_t i = 0; i < _pollfds.size ();) {
			int clientFd = _pollfds[i].fd;
			bool disconnected = false;

			if (_pollfds[i].revents & POLLIN) {
				if (clientFd == _serverFd) {
					acceptNewClient ();
				} else {
					receiveData (clientFd);
					if (_recvBuffers.find (clientFd) == _recvBuffers.end ()) {
						disconnected = true;
					}
				}
			}
			if (!disconnected && (_pollfds[i].revents & POLLOUT)) {
				sendData (clientFd);
			}
			if (!disconnected && (_pollfds[i].revents & (POLLERR | POLLHUP | POLLNVAL))) {
				disconnectClient (clientFd);
				disconnected = true;
			}
			if (!disconnected) {
				++i;
			}
		}
	}
}

void Server::queueResponse (int clientFd, const std::string &message) {
	_sendBuffers[clientFd] += message;

	// POLLOUT 監視
	for (size_t i = 0; i < _pollfds.size (); ++i) {
		if (_pollfds[i].fd == clientFd) {
			_pollfds[i].events |= POLLOUT;
			break;
		}
	}
}

void Server::processMessage (int clientFd, const std::string &message) {
	std::cout << "[FD " << clientFd << " COMMAND]: " << message << std::endl;

	Message msg = Parser::parse (message);
	if (msg.getCommand ().empty ())
		return;

	CommandResult result = CommandDispatcher::dispatch (clientFd, msg, _state);

	const std::vector<t_reply> &replies = result.getReplies ();
	for (size_t i = 0; i < replies.size (); ++i) {
		queueResponse (replies[i].fd, replies[i].reply);
	}

	if (result.shouldDisconnect ()) {
		disconnectClient (clientFd);
	}
}

void Server::disconnectClient (int clientFd) {
	std::cout << "Client disconnected. FD: " << clientFd << std::endl;

	close (clientFd);

	_recvBuffers.erase (clientFd);
	_sendBuffers.erase (clientFd);

	_state.removeClient (clientFd);

	for (std::vector<struct pollfd>::iterator it = _pollfds.begin (); it != _pollfds.end (); ++it) {
		if (it->fd == clientFd) {
			_pollfds.erase (it);
			break;
		}
	}
}

void Server::sendData (int clientFd) {
	if (_sendBuffers[clientFd].empty ()) {
		for (size_t i = 0; i < _pollfds.size (); ++i) {
			if (_pollfds[i].fd == clientFd) {
				_pollfds[i].events &= ~POLLOUT;
				break;
			}
		}
		return;
	}

	ssize_t bytesSent;
#ifdef __linux__
	bytesSent = send (clientFd, _sendBuffers[clientFd].c_str (), _sendBuffers[clientFd].length (),
					  MSG_DONTWAIT);
#else
	bytesSent =
		send (clientFd, _sendBuffers[clientFd].c_str (), _sendBuffers[clientFd].length (), 0);
#endif

	if (bytesSent > 0) {

		_sendBuffers[clientFd].erase (0, bytesSent);

		if (_sendBuffers[clientFd].empty ()) {
			for (size_t i = 0; i < _pollfds.size (); ++i) {
				if (_pollfds[i].fd == clientFd) {
					_pollfds[i].events &= ~POLLOUT;
					break;
				}
			}
		}
	} else if (bytesSent < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
			disconnectClient (clientFd);
		}
	}
}

/*
	ex)
	POLLIN  = 0001
	POLLOUT = 0100
	events  = 0101
*/
