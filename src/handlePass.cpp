#include <cctype>
#include <set>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

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
		reply = ReplyBuilder::numeric (*client, "462", "");
		result.addReply (fd, reply);
		return result;
	}
	if (!state.isCorrectPassword (msg.getSingleParam (0))) {
		reply = ReplyBuilder::numeric (*client, "464", "");
		result.addReply (fd, reply);
		return result;
	}
	client->acceptPassword ();
	if (client->tryRegister ()) {
		reply = ReplyBuilder::numeric (*client, "001",
									   client->getNickname () + "!" + client->getUsername ());
		result.addReply (fd, reply);
	}
	return result;
}
