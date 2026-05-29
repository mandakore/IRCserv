#include <iostream>
#include <string>
#include "Message.hpp"

static void expect (bool result, const std::string testCase) {
	if (result)
		std::cout << "[OK]" << testCase << std::endl;
	else
		std::cout << "[KO]" << testCase << std::endl;
}

int main () {
	std::string cmd = "sample";
	std::string p1 = "param1";
	std::string p2 = "param2";
	std::string p3 = "param3";
	std::string p4 = "param4";
	std::vector<std::string> parameters;
	parameters.push_back (p1);
	parameters.push_back (p2);
	parameters.push_back (p3);
	parameters.push_back (p4);
	Message test (cmd, parameters);
	expect (test.getCommand () == cmd, "testing getCommand");
	const std::vector<std::string> &getResult = test.getParams ();
	expect (test.getParamCount () == parameters.size (), "testing getParams");
	expect (test.getParamCount () == getResult.size (), "testing getParamCount");
	for (size_t i = 0; i < parameters.size (); ++i) {
		expect (test.getSingleParam (i) == getResult[i], "testing getParams");
		expect (test.getSingleParam (i) == parameters[i], "testing getSingleParam");
	}
	expect (test.hasParam (10) == false, "testing hasParam");
	return 0;
}
