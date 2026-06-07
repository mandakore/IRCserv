#include <sstream>
#include "ReplyBuilder.hpp"
#include "Client.hpp"

// Numeric Reply messages
const std::string ReplyBuilder::MSG_WELCOME = ":Welcome to the ircServ"; // 001
const std::string ReplyBuilder::MSG_CHANNELMODEIS = "";					 // 324
const std::string ReplyBuilder::MSG_NOTOPIC = ":No topic is set";		 // 331
const std::string ReplyBuilder::MSG_TOPIC = "";							 // 332
const std::string ReplyBuilder::MSG_INVITING = "";						 // 341
const std::string ReplyBuilder::MSG_NAMREPLY = "";						 // 353
const std::string ReplyBuilder::MSG_ENDOFNAMES = ":End of /NAMES list";	 // 366
// Numeric Error messages
const std::string ReplyBuilder::MSG_NOSUCHNICK = ":No such nick/channel";					 // 401
const std::string ReplyBuilder::MSG_NOSUCHCHANNEL = ":No such channel";						 // 403
const std::string ReplyBuilder::MSG_CANNOTSENDTOCHAN = ":Cannot send to channel";			 // 404
const std::string ReplyBuilder::MSG_NORECIPIENT = ":No recipient given";					 // 411
const std::string ReplyBuilder::MSG_NOTEXTTOSEND = ":No text to send";						 // 412
const std::string ReplyBuilder::MSG_UNKNOWNCOMMAND = ":Unknown command";					 // 421
const std::string ReplyBuilder::MSG_NONICKNAMEGIVEN = ":No nickname given";					 // 431
const std::string ReplyBuilder::MSG_ERRONEUSNICKNAME = ":Erroneous nickname";				 // 432
const std::string ReplyBuilder::MSG_NICKNAMEINUSE = ":Nickname is already in use";			 // 433
const std::string ReplyBuilder::MSG_USERNOTINCHANNEL = ":They aren't on that channel";		 // 441
const std::string ReplyBuilder::MSG_NOTONCHANNEL = ":You're not on that channel";			 // 442
const std::string ReplyBuilder::MSG_USERONCHANNEL = ":is already on channel";				 // 443
const std::string ReplyBuilder::MSG_NEEDMOREPARAMS = ":Not enough parameters";				 // 461
const std::string ReplyBuilder::MSG_ALREADYREGISTERED = ":Unauthorized command";			 // 462
const std::string ReplyBuilder::MSG_PASSWDMISMATCH = ":Password incorrect";					 // 464
const std::string ReplyBuilder::MSG_KEYSET = ":Channel key already set";					 // 467
const std::string ReplyBuilder::MSG_CHANNELISFULL = ":Cannot join channel (+l)";			 // 471
const std::string ReplyBuilder::MSG_UNKNOWNMODE = ":is unknown mode char to me for channel"; // 472
const std::string ReplyBuilder::MSG_INVITEONLYCHAN = ":Cannot join channel (+i)";			 // 473
const std::string ReplyBuilder::MSG_BADCHANNELKEY = ":Cannot join channel (+k)";			 // 475
const std::string ReplyBuilder::MSG_CHANOPRIVSNEEDED = ":You're not channel operator";		 // 482
// Unknown Numeric Code
const std::string ReplyBuilder::MSG_UNKNOWNNUMERIC = "!!UnknownNumericCode!!";

std::string ReplyBuilder::getValidTargetName (const Client &target) {
	std::string nick = target.getNickname ();
	if (nick.empty ())
		nick = "*";
	return nick;
}

std::string ReplyBuilder::getNumericMessage (const std::string &num) {
	if (num == "001")
		return ReplyBuilder::MSG_WELCOME;
	else if (num == "324")
		return ReplyBuilder::MSG_CHANNELMODEIS;
	else if (num == "331")
		return ReplyBuilder::MSG_NOTOPIC;
	else if (num == "332")
		return ReplyBuilder::MSG_TOPIC;
	else if (num == "341")
		return ReplyBuilder::MSG_INVITING;
	else if (num == "353")
		return ReplyBuilder::MSG_NAMREPLY;
	else if (num == "366")
		return ReplyBuilder::MSG_ENDOFNAMES;
	else if (num == "401")
		return ReplyBuilder::MSG_NOSUCHNICK;
	else if (num == "403")
		return ReplyBuilder::MSG_NOSUCHCHANNEL;
	else if (num == "404")
		return ReplyBuilder::MSG_CANNOTSENDTOCHAN;
	else if (num == "411")
		return ReplyBuilder::MSG_NORECIPIENT;
	else if (num == "412")
		return ReplyBuilder::MSG_NOTEXTTOSEND;
	else if (num == "421")
		return ReplyBuilder::MSG_UNKNOWNCOMMAND;
	else if (num == "431")
		return ReplyBuilder::MSG_NONICKNAMEGIVEN;
	else if (num == "432")
		return ReplyBuilder::MSG_ERRONEUSNICKNAME;
	else if (num == "433")
		return ReplyBuilder::MSG_NICKNAMEINUSE;
	else if (num == "441")
		return ReplyBuilder::MSG_USERNOTINCHANNEL;
	else if (num == "442")
		return ReplyBuilder::MSG_NOTONCHANNEL;
	else if (num == "443")
		return ReplyBuilder::MSG_USERONCHANNEL;
	else if (num == "461")
		return ReplyBuilder::MSG_NEEDMOREPARAMS;
	else if (num == "462")
		return ReplyBuilder::MSG_ALREADYREGISTERED;
	else if (num == "464")
		return ReplyBuilder::MSG_PASSWDMISMATCH;
	else if (num == "467")
		return ReplyBuilder::MSG_KEYSET;
	else if (num == "471")
		return ReplyBuilder::MSG_CHANNELISFULL;
	else if (num == "472")
		return ReplyBuilder::MSG_UNKNOWNMODE;
	else if (num == "473")
		return ReplyBuilder::MSG_INVITEONLYCHAN;
	else if (num == "475")
		return ReplyBuilder::MSG_BADCHANNELKEY;
	else if (num == "482")
		return ReplyBuilder::MSG_CHANOPRIVSNEEDED;
	else
		return ReplyBuilder::MSG_UNKNOWNNUMERIC;
}

std::string ReplyBuilder::buildPrefix (const Client &client) {
	std::string nick = client.getNickname ();
	if (nick.empty ())
		nick = "*";
	std::string user = client.getUsername ();
	if (user.empty ())
		user = "unknown";
	std::string prefix = ":" + nick + "!" + user + "@ircServ";
	return prefix;
}
