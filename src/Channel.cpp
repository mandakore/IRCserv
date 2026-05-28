#include "Channel.hpp"

Channel::Channel () {
}

Channel::Channel (const Channel &src)
	: _name (src._name), _members (src._members), _operators (src._operators),
	  _invited (src._invited), _topic (src._topic), _modes (src._modes) {
}

Channel::Channel (const std::string &name) : _name (name), _topic ("") {
}

Channel::~Channel () {
}

Channel &Channel::operator= (const Channel &other) {
	if (this != &other) {
		_name = other._name;
		_topic = other._topic;
		_members = other._members;
		_operators = other._operators;
		_invited = other._invited;
		_modes = other._modes;
	}
	return *this;
}

const std::string &Channel::getChannelName () const {
	return _name;
}

const std::string &Channel::getChannelTopic () const {
	return _topic;
}

const std::set<Client *> &Channel::getMembers () const {
	return _members;
}

size_t Channel::getMemberCount () const {
	return _members.size ();
}

const ChannelModes &Channel::getModes () const {
	return _modes;
}

ChannelModes &Channel::getModes () {
	return _modes;
}

// Setter
void Channel::setChannelTopic (const std::string &topic) {
	_topic = topic;
}

bool Channel::addMember (Client *target) {
	if (!isChannelMember (target)) {
		_members.insert (target);
		return true;
	}
	return false;
}

bool Channel::addOperator (Client *target) {
	if (isOperator (target)) {
		_operators.insert (target);
		return true;
	}
	return false;
}

bool Channel::addInvitedMember (Client *target) {
	if (isInvitedMember (target)) {
		_invited.insert (target);
		return true;
	}
	return false;
}

// Remover
bool Channel::removeMember (Client *target) {
	return (_members.erase (target) > 0);
}

bool Channel::removeOperator (Client *target) {
	return (_operators.erase (target) > 0);
}

bool Channel::removeInvitedMember (Client *target) {
	return (_invited.erase (target) > 0);
}

bool Channel::removeClient (Client *target) {
	if (!removeMember (target) || !removeOperator (target) || !removeInvitedMember (target))
		return false;
	return true;
}

// Util Functions
bool Channel::isChannelMember (Client *target) const {
	return _members.find (target) != _members.end ();
}

bool Channel::isOperator (Client *target) const {
	return _operators.find (target) != _members.end ();
}

bool Channel::isInvitedMember (Client *target) const {
	return _invited.find (target) != _members.end ();
}
