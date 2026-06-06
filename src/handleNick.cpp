#include <cctype>
#include <set>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

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
