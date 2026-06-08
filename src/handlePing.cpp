#include "Channel.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

CommandResult CommandDispatcher::_handlePing (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (!client) {
		return result;
	}
	const std::vector<std::string> &params = msg.getParams ();
	if (params.empty ()) {
		result.addReply (fd, ReplyBuilder::numeric (*client, "409", "おまかせ"));
		return result;
	}

	std::string reply = "PONG :" + params[0] + "\r\n";
	result.addReply (fd, reply);

	return result;
}