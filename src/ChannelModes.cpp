#include "ChannelModes.hpp"

ChannelModes::ChannelModes ()
	: _inviteOnly (false), _topicRestricted (false), _channelProtected (false),
	  _memberLimited (false), _maxMember (0), _channelPass ("") {
}

ChannelModes::~ChannelModes () {
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

bool ChannelModes::isMemberLimited () const {
	return _memberLimited;
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
	_memberLimited = true;
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
	_memberLimited = false;
}

void ChannelModes::unsetChannelProtected () {
	_channelProtected = false;
	_channelPass.clear ();
}
