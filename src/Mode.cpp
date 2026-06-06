#include <cctype>
#include <set>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "CommandDispatcher.hpp"
#include "Message.hpp"
#include "ReplyBuilder.hpp"
#include "ServerState.hpp"

CommandResult CommandDispatcher::_handleMode (int fd, const Message &msg, ServerState &state) {
	CommandResult result;
	Client *client = state.getClientByFd (fd);
	if (!client) {
		return result;
	}
	std::string reply;
	if (msg.getParamCount () < 1) {
		reply = ReplyBuilder::numeric (*client, "461", "MODE"); //
		result.addReply (fd, reply);
		return result;
	}
	std::string target = msg.getSingleParam (0);

	// ユーザー対象未実装
	Channel *channel = state.findChannel (target);
	if (!channel) {
		reply = ReplyBuilder::numeric (*client, "403", "MODE"); //
		result.addReply (fd, reply);
		return result;
	}

	if (msg.getParamCount() == 1){
		std::string currentMode = "+";
		// 
	}

	//
	std::string modeString = msg.getSingleParam (1);
	bool adding = true; //
	size_t paramIndex = 2;

	std::string finalModes = "";
	std::string finalParams = "";
	char lastSign = '\0';

	for (size_t i = 0; i < modeString.length (); ++i) {
		char c = modeString[i];

		if (c == '+') {
			adding = true;
		} else if (c == '-') {
			adding = false;
		} else {
			bool modeChanged = false;
			std::string modeParam = "";

			switch (c) {
				case 'i' :
				case 'k' :
				case 'l' :
				case 'o' :
				case 't' :

			default :

			}


		}
	}
	if (!finalModes.empty()) {
		std::string fullModeStr = finalModes + finalParams;
		std::string broadcastMsg = ReplyBuilder::mode(*client, target, fullModeStr);
		
		const std::set<Client*> &members = channel->getMembers();
		for (std::set<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
			result.addReply((*it)->getSocketFd(), broadcastMsg);
		}
	}
	return result;
}

// MODE <your nick>|<channel> [<mode> [<mode parameters>]]