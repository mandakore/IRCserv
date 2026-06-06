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

void CommandDispatcher::_broadcastToChannel (CommandResult &result, const Channel &channel,
											 const std::string &message, const Client *except) {
	const std::set<Client *> &members = channel.getMembers ();
	for (std::set<Client *>::const_iterator it = members.begin (); it != members.end (); ++it) {
		if (*it != NULL && *it != except)
			result.addReply ((*it)->getSocketFd (), message);
	}
	return;
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

CommandResult CommandDispatcher::_handleInvalidCommand () {
	// ErrorMsgとか詰める
	CommandResult result;
	return result;
}

CommandResult CommandDispatcher::_handlePass (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	if (msg.getParamCount () < 1) {
		reply = ReplyBuilder::numeric (*client, "461", "PASS");
		result.addReply (fd, reply);
		return result;
	}
	if (client->isRegistered ()) {
		reply = ReplyBuilder::numeric (*client, "462", "PASS");
		result.addReply (fd, reply);
		return result;
	}
	if (!state.isCorrectPassword (msg.getSingleParam (0))) {
		reply = ReplyBuilder::numeric (*client, "464", "PASS");
		result.addReply (fd, reply);
		return result;
	}
	client->acceptPassword ();
	if (client->tryRegister ()) {
		reply = ReplyBuilder::numeric (*client, "001", "");
		result.addReply (fd, reply);
	}
	return result;
}

bool CommandDispatcher::_isLetterSpecial (unsigned char c) {
	if (c == '_' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' ||
		c == '}' || c == '|')
		return true;
	return false;
}

bool CommandDispatcher::_isValidNick (const std::string &nick) {
	if (nick.empty ())
		return false;
	unsigned char c = static_cast<unsigned char> (nick[0]);
	if (!std::isalpha (c) && !_isLetterSpecial (c))
		return false;
	for (size_t i = 1; i < nick.length (); ++i) {
		c = static_cast<unsigned char> (nick[i]);
		if (!std::isalpha (c) && !std::isdigit (c) && !_isLetterSpecial (c) && c != '-')
			return false;
	}
	return true;
}

CommandResult CommandDispatcher::_handleNick (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	if (msg.getParamCount () < 1) {
		reply = ReplyBuilder::numeric (*client, "431", "NICK");
		result.addReply (fd, reply);
		return result;
	}
	std::string nick = msg.getSingleParam (0);
	if (!_isValidNick (nick)) {
		reply = ReplyBuilder::numeric (*client, "432", nick);
		result.addReply (fd, reply);
		return result;
	}
	if (state.isNickExist (nick)) {
		Client *used = state.getClientByNick (nick);
		if (used != NULL && used != client) {
			reply = ReplyBuilder::numeric (*client, "433", nick);
			result.addReply (fd, reply);
			return result;
		}
	}
	if (!state.updateNickName (*client, nick)) {
		reply = ReplyBuilder::numeric (*client, "433", nick);
		result.addReply (fd, reply);
		return result;
	}
	if (client->tryRegister ()) {
		reply = ReplyBuilder::numeric (*client, "001", "");
		result.addReply (fd, reply);
	}
	return result;
}

CommandResult CommandDispatcher::_handleUser (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	if (msg.getParamCount () < 4) {
		reply = ReplyBuilder::numeric (*client, "461", "USER");
		result.addReply (fd, reply);
		return result;
	}
	if (client->isRegistered ()) {
		reply = ReplyBuilder::numeric (*client, "462", "USER");
		result.addReply (fd, reply);
		return result;
	}
	client->setUserName (msg.getSingleParam (0));
	client->setRealName (msg.getSingleParam (3));
	if (client->tryRegister ()) {
		reply = ReplyBuilder::numeric (*client, "001", "");
		result.addReply (fd, reply);
	}
	return result;
}

bool CommandDispatcher::_isValidChannelName (const std::string &name) {
	if (name.empty ())
		return false;
	size_t len = name.length ();
	if (len == 1)
		return false;
	if (name[0] != '#')
		return false;
	unsigned char c;
	for (size_t i = 1; i < len; ++i) {
		c = static_cast<unsigned char> (name[i]);
		if (std::isspace (c) || c == ',' || c == ':' || std::iscntrl (c))
			return false;
	}
	return true;
}

void CommandDispatcher::_notifyTopic (int fd, const Client &client, const Channel &channel,
									  CommandResult &result) {
	std::string reply;
	std::string channelName = channel.getChannelName ();
	std::string topic = channel.getChannelTopic ();
	if (topic.empty ())
		reply = ReplyBuilder::numeric (client, "331", channelName);
	else
		reply = ReplyBuilder::numeric (client, "332", channelName + " :" + topic);
	result.addReply (fd, reply);
	return;
}

void CommandDispatcher::_notifyMembers (int fd, const Client &client, const Channel &channel,
										CommandResult &result) {
	const std::set<Client *> &members = channel.getMembers ();
	std::string memberLine = "";
	for (std::set<Client *>::const_iterator it = members.begin (); it != members.end (); ++it) {
		if (*it == NULL)
			continue;
		if (!memberLine.empty ())
			memberLine += " ";
		if (channel.isOperator (*it))
			memberLine += "@";
		memberLine += (*it)->getNickName ();
	}
	std::string reply;
	std::string channelName = channel.getChannelName ();
	reply = ReplyBuilder::numeric (client, "353", "= " + channelName + " :" + memberLine);
	result.addReply (fd, reply);
	reply = ReplyBuilder::numeric (client, "366", channelName + " :End of /NAMES list");
	result.addReply (fd, reply);
	return;
}

void CommandDispatcher::_joinSingleChannel (int fd, Client &client, const std::string &channel,
											const std::string &key, ServerState &state,
											CommandResult &result) {
	std::string reply;
	if (!_isValidChannelName (channel)) {
		reply = ReplyBuilder::numeric (client, "403", channel);
		result.addReply (fd, reply);
		return;
	}
	bool isNewChannel = false;
	Channel *target = state.findChannel (channel);
	if (target == NULL) {
		isNewChannel = true;
		target = state.ensureChannel (channel);
		if (target == NULL)
			return;
	}
	if (target->isChannelMember (&client))
		return;
	if (!isNewChannel) {
		if (target->getModes ().isInviteOnly ()) {
			if (!target->isInvitedMember (&client)) {
				reply = ReplyBuilder::numeric (client, "473", channel);
				result.addReply (fd, reply);
				return;
			}
		}
		if (target->getModes ().isChannelProtected ()) {
			if (!target->getModes ().checkChannelPassword (key)) {
				reply = ReplyBuilder::numeric (client, "475", channel);
				result.addReply (fd, reply);
				return;
			}
		}
		int limit = target->getModes ().getMemberLimit ();
		if (limit >= 0 && target->getMemberCount () >= limit) {
			reply = ReplyBuilder::numeric (client, "471", channel);
			result.addReply (fd, reply);
			return;
		}
	}
	if (!target->addMember (&client))
		return;
	if (isNewChannel) {
		if (!target->addOperator (&client))
			return;
	}
	reply = ReplyBuilder::join (client, channel);
	_broadcastToChannel (result, *target, reply, NULL);
	_notifyTopic (fd, client, *target, result);
	_notifyMembers (fd, client, *target, result);
	return;
}

CommandResult CommandDispatcher::_handleJoin (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	if (msg.getParamCount () < 1) {
		reply = ReplyBuilder::numeric (*client, "461", "JOIN");
		result.addReply (fd, reply);
		return result;
	}
	std::vector<std::string> channels = _splitByComma (msg.getSingleParam (0));
	std::vector<std::string> keys;
	if (msg.hasParam (1))
		keys = _splitByComma (msg.getSingleParam (1));
	for (size_t i = 0; i < channels.size (); ++i) {
		std::string key = "";
		if (i < keys.size ())
			key = keys[i];
		_joinSingleChannel (fd, *client, channels[i], key, state, result);
	}
	return result;
}

CommandResult CommandDispatcher::_handlePrivMsg (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	return result;
}

CommandResult CommandDispatcher::_handleKick (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	return result;
}

CommandResult CommandDispatcher::_handleInvite (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	return result;
}

CommandResult CommandDispatcher::_handleTopic (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	return result;
}
