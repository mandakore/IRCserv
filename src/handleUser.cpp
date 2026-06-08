#include <cctype>
#include <set>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

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
		reply = ReplyBuilder::numeric (*client, "462", "");
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
