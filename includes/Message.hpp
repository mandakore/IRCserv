#pragma once
#include <cstddef>
#include <string>
#include <vector>

class Message {
public:
	// Constructor&Destrcutor
	Message ();
	Message (const Message &src);
	Message (const std::string &command, const std::vector<std::string> &params);
	~Message ();

	// Overload Operator
	Message &operator= (const Message &src);

	// Functions
	const std::string &getCommand () const;
	const std::vector<std::string> &getParams () const;
	size_t getParamCount () const;
	const std::string &getSingleParam (size_t index) const;
	bool hasParam (size_t index) const;

private:
	std::string _command;
	std::vector<std::string> _params;
};
