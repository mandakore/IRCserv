#include "Server.hpp"

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
	// ソケットの作成
	_serverFd = socket (AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0) {
		throw std::runtime_error ("Failed to create socket");
	}

	// ソケットオプションの設定
	// TIME_WAIT状態のポートを再利用することでstruct
	// sockaddr_inを使わなくていいっぽい？←嘘。連続でサーバー立ち上げたいだけ。バインドで使ってるし
	int opt = 1;
	if (setsockopt (_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) < 0) {
		throw std::runtime_error ("Failed to set socket options");
	}

#ifdef __APPLE__
	if (fcntl (_serverFd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error ("Failed to set non-blocking mode");
	}
#endif
	// バインド(調べる)
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons (_port);

	if (bind (_serverFd, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) < 0) {
		throw std::runtime_error ("Failed to bind socket");
	}

	// リッスン(あとで調べる)
	if (listen (_serverFd, SOMAXCONN) < 0) {
		throw std::runtime_error ("Failed to listen on socket");
	}

	// poll構造体の初期化
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
	// int clientFd = _pollfds[index].fd;

#ifdef __linux__
	ssize_t bytesRead = recv (clientFd, buffer, sizeof (buffer) - 1, MSG_DONTWAIT);
#elif defined(__APPLE__)
	ssize_t bytesRead = recv (clientFd, buffer, sizeof (buffer) - 1, 0);
#endif

	if (bytesRead <= 0) {
		// 切断おrエラー
		disconnectClient(clientFd);
		// close (clientFd);
		// _pollfds.erase (_pollfds.begin () + index);
		// _recvBuffers.erase (clientFd);
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
		}
	}
}

void Server::ircLoop () {
	while (1) {
		int res = poll (&_pollfds[0], _pollfds.size (), -1);

		if (res < 0) {
			throw std::runtime_error ("Poll failed.");
		}
		for (size_t i = 0; i < _pollfds.size (); /* インクリメントは下 */) {
			int clientFd = _pollfds[i].fd;
			bool disconnected = false;

			// エラーまたは切断検知の (POLLERR, POLLHUP, POLLNVAL)
			if (_pollfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
				disconnectClient (clientFd);
				disconnected = true;
			}
			else if (_pollfds[i].revents & POLLIN) {
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
			if (!disconnected && clientFd != _serverFd) {
				++i;
			} else if (clientFd == _serverFd) {
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

	if (result.shouldDisconnect ()) {
		disconnectClient (clientFd);
		return;
	}

	const std::vector<t_reply> &replies = result.getReplies ();
	for (size_t i = 0; i < replies.size (); ++i) {
		queueResponse (replies[i].fd, replies[i].reply);
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
	}
}
