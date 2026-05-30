#include "CommandResult.hpp"

CommandResult::CommandResult () : _disconnect (false) {
}

CommandResult::CommandResult (const CommandResult &src)
	: _replies (src._replies), _disconnect (src._disconnect) {
}

CommandResult::~CommandResult () {
}

CommandResult &CommandResult::operator= (const CommandResult &src) {
	if (this != &src) {
		this->_replies = src._replies;
		this->_disconnect = src._disconnect;
	}
	return *this;
}

void CommandResult::addReply (int fd, const std::string &reply) {
	t_reply new_reply;
	new_reply.fd = fd;
	new_reply.reply = reply;
	_replies.push_back (new_reply);
}

const std::vector<t_reply> &CommandResult::getReplies () const {
	return _replies;
}

void CommandResult::requestToDisconnect () {
	_disconnect = true;
}

bool CommandResult::shouldDisconnect () const {
	return _disconnect;
}