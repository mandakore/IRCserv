#include "Channel.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

void CommandDispatcher::_kickSingleTarget (int fd, Client &client, const std::string &channelName,
										   const std::string &nick, const std::string &reason,
										   ServerState &state, CommandResult &result) {
	std::string reply;
	if (nick.empty ()) {
		reply = ReplyBuilder::numeric (client, "411", "KICK");
		result.addReply (fd, reply);
		return;
	}
	Channel *channel = state.findChannel (channelName);
	if (channel == NULL) {
		reply = ReplyBuilder::numeric (client, "403", channelName);
		result.addReply (fd, reply);
		return;
	}
	Client *target = state.getClientByNick (nick);
	if (!target) {
		reply = ReplyBuilder::numeric (client, "401", nick);
		result.addReply (fd, reply);
		return;
	}
	std::string clientName = client.getNickName ();
	if (clientName.empty ()) {
		reply = ReplyBuilder::numeric (client, "401", "");
		result.addReply (fd, reply);
		return;
	}
	if (!channel->isChannelMember (&client)) {
		reply = ReplyBuilder::numeric (client, "442", channelName);
		result.addReply (fd, reply);
		return;
	}
	if (!channel->isOperator (&client)) {
		reply = ReplyBuilder::numeric (client, "482", channelName);
		result.addReply (fd, reply);
		return;
	}
	if (!channel->isChannelMember (target)) {
		reply = ReplyBuilder::numeric (client, "441", nick + " " + channelName);
		result.addReply (fd, reply);
		return;
	}
	reply = ReplyBuilder::kick (client, *target, channelName, reason);
	_broadcastToChannel (result, *channel, reply, NULL);
	if (!channel->removeClient (target)) {
		reply = ReplyBuilder::numeric (client, "441", nick + " " + channelName);
		result.addReply (fd, reply);
		return;
	}
	state.removeChannelIfEmpty (channelName);
	return;
}

CommandResult CommandDispatcher::_handleKick (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	if (msg.getParamCount () < 2) {
		reply = ReplyBuilder::numeric (*client, "461", "KICK");
		result.addReply (fd, reply);
		return result;
	}
	std::string channelName = msg.getSingleParam (0);
	Channel *channel = state.findChannel (channelName);
	if (channel == NULL) {
		reply = ReplyBuilder::numeric (*client, "403", channelName);
		result.addReply (fd, reply);
		return result;
	}
	std::vector<std::string> nicks = _splitByComma (msg.getSingleParam (1));
	std::string reason = "";
	if (msg.hasParam (2))
		reason = msg.getSingleParam (2);
	if (reason.empty ())
		reason = client->getNickName ();
	for (size_t i = 0; i < nicks.size (); ++i) {
		_kickSingleTarget (fd, *client, channelName, nicks[i], reason, state, result);
	}
	return result;
}
