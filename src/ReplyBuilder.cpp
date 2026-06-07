#include "ReplyBuilder.hpp"
#include "Client.hpp"

static

	std::string
	ReplyBuilder::numeric (const std::string &target, const std::string &num,
						   const std::string &msg) {
	return num + " " + target + " " + msg + "\r\n";
}

std::string ReplyBuilder::numeric (const Client &client, const std::string &num,
								   const std::string &msg) {
	return num + " " + client.getNickName () + " " + msg + "\r\n";
}

std::string ReplyBuilder::join (const Client &client, const std::string &channel) {
	return ":" + client.getNickName () + " JOIN " + channel + "\r\n";
}

std::string ReplyBuilder::privMsg (const Client &from, const std::string &to,
								   const std::string &msg) {
	return ":" + from.getNickName () + " PRIVMSG " + to + " :" + msg + "\r\n";
}

std::string ReplyBuilder::kick (const Client &opUser, const Client &target,
								const std::string &channel, const std::string &reason) {
	return ":" + opUser.getNickName () + " KICK " + channel + " " + target.getNickName () + " :" +
		   reason + "\r\n";
}

std::string ReplyBuilder::invite (const Client &opUser, const Client &target,
								  const std::string &channel) {
	return ":" + opUser.getNickName () + " INVITE " + target.getNickName () + " " + channel +
		   "\r\n";
}

std::string ReplyBuilder::topic (const Client &opUser, const std::string &channel,
								 const std::string &topic) {
	return ":" + opUser.getNickName () + " TOPIC " + channel + " :" + topic + "\r\n";
}

std::string ReplyBuilder::mode (const Client &opUser, const std::string &channel,
								const std::string &modeStr) {
	return ":" + opUser.getNickName () + " MODE " + channel + " " + modeStr + "\r\n";
}
