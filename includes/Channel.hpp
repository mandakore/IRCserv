#pragma once
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include "ChannelModes.hpp"

class Client;

class Channel {
public:
	// Constructor&Destructor
	Channel (const std::string &name);
	~Channel ();

	// Functions
	// Getter
	const std::string &getChannelName () const;
	const std::string &getChannelTopic () const;
	const std::set<Client *> &getMembers () const;
	size_t getMemberCount () const;
	const ChannelModes &getModes () const;
	ChannelModes &getModes ();

	// Setter
	void setChannelTopic (const std::string &topic);
	bool addMember (Client *target);
	bool addOperator (Client *target);
	bool addInvitedMember (Client *target);

	// Remover
	bool removeMember (Client *target);
	bool removeOperator (Client *target);
	bool removeInvitedMember (Client *target);
	bool removeClient (Client *target);

	// Util Functions
	bool isChannelMember (const Client *target) const;
	bool isOperator (const Client *target) const;
	bool isInvitedMember (const Client *target) const;

private:
	// Channel Values
	std::string _name;
	std::string _topic;
	ChannelModes _modes;

	// Member Lists
	std::set<Client *> _members;
	std::set<Client *> _operators;
	std::set<Client *> _invited;

	// Forbidden OCF Functions
	Channel ();
	Channel (const Channel &src);
	Channel &operator= (const Channel &src);
};
