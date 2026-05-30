#pragma once

#include <string>
#include <vector>

struct t_reply {
	int fd;
	std::string reply;
};

class CommandResult {
public:
	// Constructor&Destructor
	CommandResult ();
	CommandResult (const CommandResult &src);
	~CommandResult ();

	// Overload Operator
	CommandResult &operator= (const CommandResult &src);

	// Functions
	void addReply (int fd, const std::string &reply);
	const std::vector<t_reply> &getReplies () const;
	void requestToDisconnect ();
	bool shouldDisconnect () const;

private:
	std::vector<t_reply> _replies;
	bool _disconnect;
};
