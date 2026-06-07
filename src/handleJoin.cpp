#include <cctype>
#include <set>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

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
