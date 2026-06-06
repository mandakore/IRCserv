#include "Channel.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

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
		reason = "Default Msg";
	for (size_t i = 0; i < nicks.size (); ++i) {
		_kickSingleTarget (fd, *client, channelName, nicks[i], reason, state, result);
	}
	return result;
}
