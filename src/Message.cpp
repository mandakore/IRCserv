#include "Message.hpp"

Message::Message (const Message &src) : _command (src._command), _params (src._params) {
}

Message::Message (const std::string &command, const std::vector<std::string> &params)
	: _command (command), _params (params) {
}

Message::~Message () {
}

const std::string &Message::getCommand () const {
	return _command;
}

const std::vector<std::string> &Message::getParams () const {
	return _params;
}

size_t Message::getParamCount () const {
	return _params.size ();
}

const std::string &Message::getSingleParam (size_t index) const {
	return _params[index];
}

bool Message::hasParam (size_t index) const {
	if (index < _params.size ())
		return true;
	else
		return false;
}
