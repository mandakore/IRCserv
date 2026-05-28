#pragma once
#include <string>

class ChannelModes {
public:
	// Constructor&Destructor
	ChannelModes ();
	ChannelModes (const ChannelModes &src);
	~ChannelModes ();

	// Overload Operator
	ChannelModes &operator= (const ChannelModes &src);

	// Functions
	// Getters
	bool isInviteOnly () const;
	bool isTopicRestricted () const;
	bool isChannelProtected () const;

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
	bool _inviteOnly;
	bool _topicRestricted;
	bool _channelProtected;
	int _maxMember;
	std::string _channelPass;
};
