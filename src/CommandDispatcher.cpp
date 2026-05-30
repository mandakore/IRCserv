#include "CommandDispatcher.hpp"

CommandDispatcher::cmds[0] = "INVALID";
CommandDispatcher::cmds[1] = "PASS";
CommandDispatcher::cmds[2] = "NICK";
CommandDispatcher::cmds[3] = "USER";
CommandDispatcher::cmds[4] = "JOIN";
CommandDispatcher::cmds[5] = "PRIVMSG";
CommandDispatcher::cmds[6] = "KICK";
CommandDispatcher::cmds[7] = "INVITE";
CommandDispatcher::cmds[8] = "TOPIC";
CommandDispatcher::cmds[9] = "MODE";

CommandResult CommandDispatcher::dispatch (int fd, const Message &msg, ServerState &state) {
	std::string cmdName = msg->getCommand ();
	int cmdNumber = cmdNameToNumber (CommandDispatcher::cmds, cmdName);
	switch (cmdNumber) {
		case 1
			return handlePass(fd, msg, state);
		case 2
			return handleNick(fd, msg, state);
		case 3
			return handleUser(fd, msg, state);
		case 4
			return handleJoin(fd, msg, state);
		case 5
			return handlePrivMsg(fd, msg, state);
		case 6
			return handleKick(fd, msg, state);
		case 7
			return handleInvite(fd, msg, state);
		case 8
			return handleTopic(fd, msg, state);
		case 9
			return handleMode(fd, msg, state);
		default
			return handleInvalidCommand();
	}
}

int CommandDispatcher::_nameToNumber (const std::string cmds[], const std::string &cmd) {
	for (int i = 0; i < cmdCount; ++i) {
		if (cmds[i] == cmd)
			return i;
	}
	return 0;
}

CommandResult CommandDispatcher::_handleInvalidCommand()
{
	//ErrorMsgとか詰める
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
