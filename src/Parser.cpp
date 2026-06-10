#include "Parser.hpp"
#include <cctype>
#include <algorithm>

static std::string trim (const std::string &s) {
	const std::string::size_type start = s.find_first_not_of (' ');
	if (start == std::string::npos)
		return "";
	const std::string::size_type end = s.find_last_not_of (' ');
	return s.substr (start, end - start + 1);
}

static std::string toUpper (const std::string &s) {
	std::string result (s);
	for (std::string::size_type i = 0; i < result.size (); ++i)
		result[i] = static_cast<char> (std::toupper (static_cast<unsigned char> (result[i])));
	return result;
}

Message Parser::parse (const std::string &line) {
	const int MAX_PARAMS = 15;

	// 空もしくは空白のみ
	if (trim (line).empty ())
		return Message ("", std::vector<std::string> ());

	std::string::size_type pos = 0;
	const std::string::size_type len = line.size ();

	if (pos < len && line[pos] == ':') {
		while (pos < len && line[pos] != ' ')
			++pos;
		while (pos < len && line[pos] == ' ')
			++pos;
	}

	std::string::size_type cmdStart = pos;
	while (pos < len && line[pos] != ' ')
		++pos;

	std::string command = toUpper (line.substr (cmdStart, pos - cmdStart));

	if (command.empty ())
		return Message ("", std::vector<std::string> ());

	std::vector<std::string> params;

	while (pos < len && static_cast<int> (params.size ()) < MAX_PARAMS) {
		while (pos < len && line[pos] == ' ')
			++pos;
		if (pos >= len)
			break;

		// : 以降はtrailingとみなす
		if (line[pos] == ':') {
			params.push_back (line.substr (pos + 1));
			break;
		}

		std::string::size_type paramStart = pos;
		while (pos < len && line[pos] != ' ')
			++pos;

		params.push_back (line.substr (paramStart, pos - paramStart));
	}
	return Message (command, params);
}
