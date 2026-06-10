#pragma once
#include <string>

class ChannelModes {
public:
	// Constructor&Destructor
	ChannelModes ();
	~ChannelModes ();

	// Functions
	// Getters
	bool isInviteOnly () const;
	bool isTopicRestricted () const;
	bool isChannelProtected () const;
	bool isMemberLimited () const;

	int getMemberLimit () const;
	bool checkChannelPassword (const std::string &input) const;

	// Setters
	void setInviteOnly ();
	void setTopicRestricted ();
	void setMemberLimit (int limit);
	void setChannelProtected (const std::string &password);

	// unset
	void unsetInviteOnly ();
	void unsetTopicRestricted ();
	void unsetMemberLimit ();
	void unsetChannelProtected ();

private:
	// Flags
	bool _inviteOnly;
	bool _topicRestricted;
	bool _channelProtected;
	bool _memberLimited;

	// Values
	size_t _maxMember;
	std::string _channelPass;

	// Forbidden OCF Functions
	ChannelModes (const ChannelModes &src);
	ChannelModes &operator= (const ChannelModes &src);
};
