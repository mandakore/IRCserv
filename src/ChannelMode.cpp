#include "ChannelModes.hpp"

ChannelModes::ChannelModes ()
	: _inviteOnly (false), _topicRestricted (false), _channelProtected (false), _maxMember (-1),
	  _channelPass ("") {
}

ChannelModes::ChannelModes (const ChannelModes &src)
	: _inviteOnly (src._inviteOnly), _topicRestricted (src._topicRestricted),
	  _channelProtected (src._channelProtected), _maxMember (src._maxMember),
	  _channelPass (src._channelPass) {
}

ChannelModes::~ChannelModes () {
}

ChannelModes &ChannelModes::operator= (const ChannelModes &src) {
	if (this != &src) {
		_inviteOnly = src._inviteOnly;
		_topicRestricted = src._topicRestricted;
		_channelProtected = src._channelProtected;
		_maxMember = src._maxMember;
		_channelPass = src._channelPass;
	}
	return *this;
}

bool ChannelModes::isInviteOnly () const {
	return _inviteOnly;
}

bool ChannelModes::isTopicRestricted () const {
	return _topicRestricted;
}

bool ChannelModes::isChannelProtected () const {
	return _channelProtected;
}

int ChannelModes::getMemberLimit () const {
	return _maxMember;
}

bool ChannelModes::checkChannelPassword (const std::string &input) const {
	if (input != _channelPass) {
		return false;
	}
	return true;
}

// Setters
void ChannelModes::setInviteOnly () {
	_inviteOnly = true;
}

void ChannelModes::setTopicRestricted () {
	_topicRestricted = true;
}

void ChannelModes::setMemberLimit (int limit) {
	_maxMember = limit;
}

void ChannelModes::setChannelProtected (const std::string &password) {
	_channelProtected = true;
	_channelPass = password;
}

// unset
void ChannelModes::unsetInviteOnly () {
	_inviteOnly = false;
}

void ChannelModes::unsetTopicRestricted () {
	_topicRestricted = false;
}

void ChannelModes::unsetMemberLimit () {
	_maxMember = -1;
}

void ChannelModes::unsetChannelProtected () {
	_channelProtected = false;
}
