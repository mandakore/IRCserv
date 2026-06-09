#include "Channel.hpp"

Channel::Channel (const std::string &name) : _name (name), _topic ("") {
}

Channel::~Channel () {
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
	if (!isOperator (target)) {
		_operators.insert (target);
		return true;
	}
	return false;
}

bool Channel::addInvitedMember (Client *target) {
	if (!isInvitedMember (target)) {
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
	bool removed = false;
	if (removeMember (target))
		removed = true;
	if (removeOperator (target))
		removed = true;
	if (removeInvitedMember (target))
		removed = true;
	return removed;
}

// Util Functions
bool Channel::isChannelMember (const Client *target) const {
	for (std::set<Client *>::const_iterator it = _members.begin (); it != _members.end (); ++it) {
		if (*it == target)
			return true;
	}
	return false;
}

bool Channel::isOperator (const Client *target) const {
	for (std::set<Client *>::const_iterator it = _operators.begin (); it != _operators.end ();
		 ++it) {
		if (*it == target)
			return true;
	}
	return false;
}

bool Channel::isInvitedMember (const Client *target) const {
	for (std::set<Client *>::const_iterator it = _invited.begin (); it != _invited.end (); ++it) {
		if (*it == target)
			return true;
	}
	return false;
}
