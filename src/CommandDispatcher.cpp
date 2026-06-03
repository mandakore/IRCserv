#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ServerState.hpp"
#include "Client.hpp"
#include "ReplyBuilder.hpp"

const std::string CommandDispatcher::cmds[CommandDispatcher::CMD_COUNT] = {
	"INVALID", "PASS", "NICK", "USER", "JOIN", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE"};

CommandResult CommandDispatcher::dispatch (int fd, const Message &msg, ServerState &state) {
	std::string cmdName = msg.getCommand ();
	int cmdNumber = _cmdNameToNumber (CommandDispatcher::cmds, cmdName);
	switch (cmdNumber) {
	case CMD_PASS:
		return _handlePass (fd, msg, state);
	case CMD_NICK:
		return _handleNick (fd, msg, state);
	case CMD_USER:
		return _handleUser (fd, msg, state);
	case CMD_JOIN:
		return _handleJoin (fd, msg, state);
	case CMD_PRIVMSG:
		return _handlePrivMsg (fd, msg, state);
	case CMD_KICK:
		return _handleKick (fd, msg, state);
	case CMD_INVITE:
		return _handleInvite (fd, msg, state);
	case CMD_TOPIC:
		return _handleTopic (fd, msg, state);
	case CMD_MODE:
		return _handleMode (fd, msg, state);
	default:
		return _handleInvalidCommand ();
	}
}

int CommandDispatcher::_cmdNameToNumber (const std::string cmds[], const std::string &cmd) {
	for (int i = 0; i < CMD_COUNT; ++i) {
		if (cmds[i] == cmd)
			return i;
	}
	return CMD_INVALID;
}

CommandResult CommandDispatcher::_handleInvalidCommand () {
	// ErrorMsgとか詰める
}

CommandResult CommandDispatcher::_handlePass (int fd, const Message &msg, ServerState &state) {
	CommandResult	result;
	Client			*client = state.getClientByFd(fd);
	if (client == NULL)
		return result;
	std::string	reply;
	if (msg.getParamCount() < 1)
	{
		reply = ReplyBuilder::numeric(*client, "461", "PASS");
		result.addReply(fd, reply);
		return result;
	}
	if (client->isRegistered())
	{
		reply = ReplyBuilder::numeric(*client, "462", "PASS");
		result.addReply(fd, reply);
		return result;
	}
	if (!msg.hasParam(0) || !state.isCorrectPassword(msg.getSingleParam(0)))
	{
		reply = ReplyBuilder::numeric(*client, "464", "PASS");
		result.addReply(fd, reply);
		return result;
	}
	client->acceptPassword();
	if (client->tryRegister())
	{
		reply = ReplyBuilder::numeric(*client, "001", "");
		result.addReply(fd, reply);
	}
	return result;
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
