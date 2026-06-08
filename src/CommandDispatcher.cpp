#include <cctype>
#include <set>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

const std::string CommandDispatcher::cmds[CommandDispatcher::CMD_COUNT] = {
	"INVALID", "PASS", "NICK", "USER", "JOIN", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE", "PING"};

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
	// Add IRSSI Commands
	default:
		return _handleInvalidCommand (fd, msg, state);
	}
}

std::vector<std::string> CommandDispatcher::_splitByComma (const std::string &target) {
	std::vector<std::string> result;
	size_t start = 0;
	for (size_t i = 0; i < target.length (); ++i) {
		if (target[i] == ',') {
			result.push_back (target.substr (start, i - start));
			start = i + 1;
		}
	}
	result.push_back (target.substr (start));
	return result;
}

int CommandDispatcher::_cmdNameToNumber (const std::string cmds[], const std::string &cmd) {
	for (int i = 0; i < CMD_COUNT; ++i) {
		if (cmds[i] == cmd)
			return i;
	}
	return CMD_INVALID;
}

void CommandDispatcher::_broadcastToChannel (CommandResult &result, const Channel &channel,
											 const std::string &message, const Client *except) {
	const std::set<Client *> &members = channel.getMembers ();
	for (std::set<Client *>::const_iterator it = members.begin (); it != members.end (); ++it) {
		if (*it != NULL && *it != except)
			result.addReply ((*it)->getSocketFd (), message);
	}
	return;
}

CommandResult CommandDispatcher::_handleInvalidCommand (int fd, const Message &msg,
														ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply = ReplyBuilder::numeric (*client, "421", msg.getCommand ());
	result.addReply (fd, reply);
	return result;
}
