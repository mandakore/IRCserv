#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

CommandResult CommandDispatcher::_handleTopic (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;
	std::string reply;
	size_t param = msg.getParamCount ();
	if (param < 1) {
		reply = ReplyBuilder::numeric (*client, "461", "TOPIC");
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
	if (!channel->isChannelMember (client)) {
		reply = ReplyBuilder::numeric (*client, "442", channelName);
		result.addReply (fd, reply);
		return result;
	}
	std::string currentTopic = channel->getChannelTopic ();
	if (param == 1) {
		if (currentTopic.empty ())
			reply = ReplyBuilder::numeric (*client, "331", channelName);
		else
			reply = ReplyBuilder::numeric (*client, "332", channelName + " :" + currentTopic);
		result.addReply (fd, reply);
		return result;
	}
	if (channel->getModes ().isTopicRestricted () && !channel->isOperator (client)) {
		reply = ReplyBuilder::numeric (*client, "482", channelName);
		result.addReply (fd, reply);
		return result;
	}
	std::string newTopic = msg.getSingleParam (1);
	channel->setChannelTopic (newTopic);
	reply = ReplyBuilder::topic (*client, channelName, newTopic);
	_broadcastToChannel (result, *channel, reply, NULL);
	return result;
}
