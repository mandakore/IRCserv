#pragma once
#include <string>
#include "Message.hpp"

class Parser {
public:
	// Constructor&Destructor
	Parser ();
	Parser (const Parser &src);
	~Parser ();

	// Overload Operator
	Parser &operator= (const Parser &src);

	// Functions
	static Message parse (const std::string &line);

private:
};
