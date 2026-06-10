#pragma once
#include <string>
#include "Message.hpp"

class Parser {
public:
	// Functions
	static Message parse (const std::string &line);

private:
	// Forbidden OCF Functions
	Parser ();
	Parser (const Parser &src);
	~Parser ();
	Parser &operator= (const Parser &src);
};
