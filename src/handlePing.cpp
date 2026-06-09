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
	std::string token = params.empty () ? "ircServ" : params[0];
	std::string reply = "PONG ircServ :" + token + "\r\n";
	result.addReply (fd, reply);

	return result;
}
