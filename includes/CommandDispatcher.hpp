#pragma once

#include <string>
#include "CommandResult.hpp"

class Message;
class ServerState;

class CommandDispatcher {
public:
	// Functions
	static CommandResult dispatch (int fd, const Message &msg, ServerState &state);

private:
	//
	static const int cmdsCount = 10;
	static const std::string cmds[cmdsCount];
	// Command handleres
	static int _cmdNameToNumber (const std::string cmds[], const std::string &cmd);
	static CommandResult _handleInvalidCommand ();
	static CommandResult _handlePass (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleNick (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleUser (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleJoin (int fd, const Message &msg, ServerState &state);
	static CommandResult _handlePrivMsg (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleKick (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleInvite (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleTopic (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleMode (int fd, const Message &msg, ServerState &state);

	// Forbidden OCF Functions
	CommandDispatcher ();
	CommandDispatcher (const CommandDispatcher &src);
	~CommandDispatcher ();
	CommandDispatcher &operator= (const CommandDispatcher &src);
};
