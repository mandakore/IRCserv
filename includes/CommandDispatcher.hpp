#pragma once

#include "CommandResult.hpp"

class Message;
class ServerState;

class CommandDispatcher {
public:
	// Constructor&Destructor
	CommandDispatcher ();
	CommandDispatcher (const CommandDispatcher &src);
	~CommandDispatcher ();

	// Overload Operator
	CommandDispatcher &operator= (const CommandDispatcher &src);

	// Functions
	CommandResult dispatch (int fd, const Message &msg, ServerState &state);

private:
	CommandResult handlePass (int fd, const Message &msg, ServerState &state);
	CommandResult handleNick (int fd, const Message &msg, ServerState &state);
	CommandResult handleUser (int fd, const Message &msg, ServerState &state);
	CommandResult handleJoin (int fd, const Message &msg, ServerState &state);
	CommandResult handlePrivMsg (int fd, const Message &msg, ServerState &state);
	CommandResult handleKick (int fd, const Message &msg, ServerState &state);
	CommandResult handleInvite (int fd, const Message &msg, ServerState &state);
	CommandResult handleTopic (int fd, const Message &msg, ServerState &state);
	CommandResult handleMode (int fd, const Message &msg, ServerState &state);
};
