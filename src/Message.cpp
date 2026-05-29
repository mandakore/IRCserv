#include "Message.hpp"

Message::Message () : _command (""), _params () {
}

Message::Message (const Message &src) : _command (src._command), _params (src._params) {
}

Message::Message (const std::string &command, const std::vector<std::string> &params)
	: _command (command), _params (params) {
}

Message::~Message () {
}

Message &Message::operator= (const Message &src) {
	if (this != &src) {
		_command = src._command;
		_params = src._params;
	}
	return *this;
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

bool Message::hasParam (size_t index) const {
	return index < _params.size ();
}

//  範囲外はから文字の参照
// 空文字列はstatic(UB)
const std::string &Message::getSingleParam (size_t index) const {
	static const std::string empty ("");
	if (index >= _params.size ())
		return empty;
	return _params[index];
}
