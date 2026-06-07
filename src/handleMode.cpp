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
		reply = ReplyBuilder::numeric (*client, "461", "MODE");
		result.addReply (fd, reply);
		return result;
	}
	std::string target = msg.getSingleParam (0);

	Channel *channel = state.findChannel (target);
	if (!channel) {
		reply = ReplyBuilder::numeric (*client, "403", target);
		result.addReply (fd, reply);
		return result;
	}

	if (msg.getParamCount () == 1) {
		std::string currentModes = "+";
		if (channel->getModes ().isInviteOnly ()) {
			currentModes += "i";
		}
		if (channel->getModes ().isTopicRestricted ()) {
			currentModes += "t";
		}
		if (channel->getModes ().isChannelProtected ()) {
			currentModes += "k";
		}
		if (channel->getModes ().getMemberLimit () > 0) {
			currentModes += "l";
		}
		std::string reply = ReplyBuilder::numeric (*client, "324", target + " :" + currentModes);
		result.addReply (fd, reply);
		return result;
	}

	if (!channel->isChannelMember (client)) {
		std::string reply = ReplyBuilder::numeric (*client, "442", target);
		result.addReply (fd, reply);
		return result;
	}

	if (!channel->isOperator (client)) {
		std::string reply = ReplyBuilder::numeric (*client, "482", target);
		result.addReply (fd, reply);
		return result;
	}

	std::string modeString = msg.getSingleParam (1);
	bool adding = true;
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
			case 'i':
				if (adding && !channel->getModes ().isInviteOnly ()) {
					channel->getModes ().setInviteOnly ();
					modeChanged = true;
				} else if (!adding && channel->getModes ().isInviteOnly ()) {
					channel->getModes ().unsetInviteOnly ();
					modeChanged = true;
				}
				break;

			case 't':
				if (adding && !channel->getModes ().isTopicRestricted ()) {
					channel->getModes ().setTopicRestricted ();
					modeChanged = true;
				} else if (!adding && channel->getModes ().isTopicRestricted ()) {
					channel->getModes ().unsetTopicRestricted ();
					modeChanged = true;
				}
				break;

			case 'k':
				if (adding) {
					if (paramIndex < msg.getParamCount ()) {
						modeParam = msg.getSingleParam (paramIndex++);
						channel->getModes ().setChannelProtected (modeParam);
						modeChanged = true;
					}
				} else {
					if (channel->getModes ().isChannelProtected ()) {
						channel->getModes ().unsetChannelProtected ();
						modeChanged = true;

						if (paramIndex < msg.getParamCount ())
							paramIndex++;
					}
				}
				break;

			case 'l':
				if (adding) {
					if (paramIndex < msg.getParamCount ()) {
						modeParam = msg.getSingleParam (paramIndex++);
						int limit = std::atoi (modeParam.c_str ());
						if (limit > 0) {
							channel->getModes ().setMemberLimit (limit);
							modeChanged = true;
						}
					}
				} else {
					if (channel->getModes ().getMemberLimit () > 0) {
						channel->getModes ().unsetMemberLimit ();
						modeChanged = true;
					}
				}
				break;

			case 'o':
				if (paramIndex < msg.getParamCount ()) {
					modeParam = msg.getSingleParam (paramIndex++);
					Client *targetClient = state.getClientByNick (modeParam);
					if (!targetClient) {
						std::string reply = ReplyBuilder::numeric (*client, "401", target);
						result.addReply (fd, reply);
						return result;
					}
					if (targetClient && channel->isChannelMember (targetClient)) {
						if (adding) {
							if (channel->addOperator (targetClient))
								modeChanged = true;
						} else {
							if (channel->removeOperator (targetClient))
								modeChanged = true;
						}
					} else {
						std::string reply = ReplyBuilder::numeric (*client, "441", target);
						result.addReply (fd, reply);
					}
				}
				break;

			default:
				std::string reply = ReplyBuilder::numeric (*client, "472", "MODE");
				result.addReply (fd, reply);
				break;
			}
			if (modeChanged) {
				char currentSign = adding ? '+' : '-';
				if (currentSign != lastSign) {
					finalModes += currentSign;
					lastSign = currentSign;
				}
				finalModes += c;
				if (!modeParam.empty ()) {
					finalParams += " " + modeParam;
				}
			}
		}
	}
	if (!finalModes.empty ()) {
		std::string fullModeStr = finalModes + finalParams;
		std::string broadcastMsg = ReplyBuilder::mode (*client, target, fullModeStr);
		_broadcastToChannel (result, *channel, broadcastMsg, NULL);
	}
}
return result;
}

// MODE <your nick>|<channel> [<mode> [<mode parameters>]]
