#include "Channel.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

CommandResult CommandDispatcher::_handleInvite (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	if (msg.getParamCount () < 2) {
		reply = ReplyBuilder::numeric (*client, "461", "INVITE");
		result.addReply (fd, reply);
		return result;
	}
	std::string channelName = msg.getSingleParam (1);
	Channel *channel = state.findChannel (channelName);
	if (channel == NULL) {
		reply = ReplyBuilder::numeric (*client, "403", channelName);
		result.addReply (fd, reply);
		return result;
	}
	if (!channel->isChannelMember (client)) {
		reply = ReplyBuilder::numeric (*client, "442", channelName);
		result.addReply (fd, reply);
		return result;
	}
	if (!channel->isOperator (client)) {
		reply = ReplyBuilder::numeric (*client, "482", channelName);
		result.addReply (fd, reply);
		return result;
	}
	std::string nick = msg.getSingleParam (0);
	Client *target = state.getClientByNick (nick);
	if (target == NULL) {
		reply = ReplyBuilder::numeric (*client, "401", nick);
		result.addReply (fd, reply);
		return result;
	}
	if (channel->isChannelMember (target)) {
		reply = ReplyBuilder::numeric (*client, "443", nick + " " + channelName);
		result.addReply (fd, reply);
		return result;
	}
	if (!channel->addInvitedMember (target))
		return result;
	reply = ReplyBuilder::numeric (*client, "341", nick + " " + channelName);
	result.addReply (fd, reply);
	reply = ReplyBuilder::invite (*client, *target, channelName);
	result.addReply (target->getSocketFd (), reply);
	return result;
}
