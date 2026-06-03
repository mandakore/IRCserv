#include "CommandDispatcher.hpp"

const std::string CommandDispatcher::cmds[CommandDispatcher::cmdsCount] = {
	"INVALID", "PASS", "NICK", "USER", "JOIN", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE"};

CommandResult CommandDispatcher::dispatch (int fd, const Message &msg, ServerState &state) {
	std::string cmdName = msg.getCommand ();
	int cmdNumber = _cmdNameToNumber (CommandDispatcher::cmds, cmdName);
	switch (cmdNumber) {
	case 1:
		return _handlePass (fd, msg, state);
	case 2:
		return _handleNick (fd, msg, state);
	case 3:
		return _handleUser (fd, msg, state);
	case 4:
		return _handleJoin (fd, msg, state);
	case 5:
		return _handlePrivMsg (fd, msg, state);
	case 6:
		return _handleKick (fd, msg, state);
	case 7:
		return _handleInvite (fd, msg, state);
	case 8:
		return _handleTopic (fd, msg, state);
	case 9:
		return _handleMode (fd, msg, state);
	default:
		return _handleInvalidCommand ();
	}
}

int CommandDispatcher::_cmdNameToNumber (const std::string cmds[], const std::string &cmd) {
	for (int i = 0; i < cmdsCount; ++i) {
		if (cmds[i] == cmd)
			return i;
	}
	return 0;
}

CommandResult CommandDispatcher::_handleInvalidCommand () {
	// ErrorMsgとか詰める
}

CommandResult CommandDispatcher::_handlePass (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handleNick (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handleUser (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handleJoin (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handlePrivMsg (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handleKick (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handleInvite (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handleTopic (int fd, const Message &msg, ServerState &state) {
}

CommandResult CommandDispatcher::_handleMode (int fd, const Message &msg, ServerState &state) {
}
