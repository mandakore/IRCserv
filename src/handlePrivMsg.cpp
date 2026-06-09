#include "Channel.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

void CommandDispatcher::_sendPrivMsgToTarget (int fd, const Client &client,
											  const std::string &target, const std::string &msg,
											  const ServerState &state, CommandResult &result) {
	std::string reply = "";
	if (target.empty ()) {
		reply = ReplyBuilder::numeric (client, "411", "PRIVMSG");
		result.addReply (fd, reply);
		return;
	}
	if (target[0] == '#') {
		Channel *channel = state.findChannel (target);
		if (channel == NULL) {
			reply = ReplyBuilder::numeric (client, "403", target);
			result.addReply (fd, reply);
			return;
		}
		if (!channel->isChannelMember (&client)) {
			reply = ReplyBuilder::numeric (client, "404", target);
			result.addReply (fd, reply);
			return;
		}
		reply = ReplyBuilder::privMsg (client, target, msg);
		_broadcastToChannel (result, *channel, reply, &client);
	} else {
		Client *targetClient = state.getClientByNick (target);
		if (targetClient == NULL) {
			reply = ReplyBuilder::numeric (client, "401", target);
			result.addReply (fd, reply);
			return;
		}
		int targetFd = targetClient->getSocketFd ();
		reply = ReplyBuilder::privMsg (client, target, msg);
		result.addReply (targetFd, reply);
	}
	return;
}

CommandResult CommandDispatcher::_handlePrivMsg (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	if (msg.getParamCount () < 1) {
		reply = ReplyBuilder::numeric (*client, "411", "PRIVMSG");
		result.addReply (fd, reply);
		return result;
	}
	if (msg.getParamCount () < 2 || msg.getSingleParam (1).empty ()) {
		reply = ReplyBuilder::numeric (*client, "412", "");
		result.addReply (fd, reply);
		return result;
	}
	std::vector<std::string> targets = _splitByComma (msg.getSingleParam (0));
	std::string message = msg.getSingleParam (1);
	for (size_t i = 0; i < targets.size (); ++i)
		_sendPrivMsgToTarget (fd, *client, targets[i], message, state, result);
	return result;
}
