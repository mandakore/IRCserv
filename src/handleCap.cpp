#include <cctype>
#include <set>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

CommandResult CommandDispatcher::_handleCap (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (client == NULL)
		return result;

	if (msg.getParamCount () < 1) {
		return result;
	}

	std::string subCommand = msg.getSingleParam (0);
	if (subCommand == "LS") {
		std::string target = client->getNickName ().empty () ? "*" : client->getNickName ();
		result.addReply (fd, ":ircServ CAP " + target + " LS :\r\n");
	} else if (subCommand == "REQ") {
		std::string requested = msg.getParamCount () > 1 ? msg.getSingleParam (1) : "";
		std::string target = client->getNickName ().empty () ? "*" : client->getNickName ();
		result.addReply (fd, ":ircServ CAP " + target + " NAK :" + requested + "\r\n");
	} else if (subCommand == "END") {
		// なにもしない？
	}
	return result;
}
