#include "ChannelModes.hpp"

ChannelModes::ChannelModes ()
	: _inviteOnly (false), _topicRestricted (false), _memberLimited (false),
	  _channelProtected (false), _maxMember (-1), _channelPass ("") {
}

ChannelModes::ChannelModes (const ChannelModes &src) {
}

ChannelModes::~ChannelModes () {
}

ChannelModes &ChannelModes::operator= (const ChannelModes &src) {
	if (this != &src) {
		_inviteOnly = src._inviteOnly;
		_topicRestricted = src._topicRestricted;
		_memberLimited = src._memberLimited;
		_channelProtected = src._channelProtected;
		_maxMember = src._maxMember;
		_channelPass = src._channelPass;
	}
	return *this;
}

bool ChannelModes::isInviteOnly () const {
}

bool ChannelModes::isTopicRestricted () const {
}

bool ChannelModes::isMemberLimited () const {
}

bool ChannelModes::isChannelProtected () const {
}

int ChannelModes::getMemberLimit () const {
}

bool ChannelModes::checkChannelPassword (const std::string &input) const {
}

// Setters
void ChannelModes::setInviteOnly () {
}

void ChannelModes::setTopicRestricted () {
}

void ChannelModes::setMemberLimit (int limit) {
}

void ChannelModes::setChannelProtected (const std::string &password) {
}

// unset
void ChannelModes::unsetInviteOnly () {
}

void ChannelModes::unsetTopicRestricted () {
}

void ChannelModes::unsetMemberLimit () {
}

void ChannelModes::unsetChannelProtected () {
}
