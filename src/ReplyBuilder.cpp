#include "Client.hpp"
#include "ReplyBuilder.hpp"

std::string ReplyBuilder::numeric (const std::string &target, const std::string &num,
								   const std::string &msg) {
	std::string numericMsg = getNumericMessage (num);
	if (numericMsg == MSG_UNKNOWNNUMERIC)
		return "";
	std::string validTarget = target;
	if (validTarget.empty ())
		validTarget = "*";
	std::string reply = ":ircServ " + num + " " + validTarget;
	if (!msg.empty ())
		reply += (" " + msg);
	if (!numericMsg.empty ())
		reply += (" " + numericMsg);
	reply += "\r\n";
	return reply;
}

std::string ReplyBuilder::numeric (const Client &client, const std::string &num,
								   const std::string &msg) {
	std::string target = getValidTargetName (client);
	std::string reply = numeric (target, num, msg);
	return reply;
}

std::string ReplyBuilder::join (const Client &client, const std::string &channel) {
	std::string reply = buildPrefix (client);
	reply += " JOIN ";
	reply += channel;
	reply += "\r\n";
	return reply;
}

std::string ReplyBuilder::privMsg (const Client &from, const std::string &to,
								   const std::string &msg) {
	std::string reply = buildPrefix (from);
	reply += " PRIVMSG ";
	reply += to;
	if (!msg.empty ())
		reply += (" :" + msg);
	reply += "\r\n";
	return reply;
}

std::string ReplyBuilder::kick (const Client &opUser, const Client &target,
								const std::string &channel, const std::string &reason) {
	std::string reply = buildPrefix (opUser);
	reply += " KICK ";
	reply += channel;
	reply += " ";
	reply += getValidTargetName (target);
	if (!reason.empty ()) {
		reply += " :";
		reply += reason;
	}
	reply += "\r\n";
	return reply;
}

std::string ReplyBuilder::invite (const Client &opUser, const Client &target,
								  const std::string &channel) {
	std::string reply = buildPrefix (opUser);
	reply += " INVITE ";
	reply += getValidTargetName (target);
	reply += (" " + channel);
	reply += "\r\n";
	return reply;
}

std::string ReplyBuilder::topic (const Client &opUser, const std::string &channel,
								 const std::string &topic) {
	std::string reply = buildPrefix (opUser);
	reply += " TOPIC ";
	reply += channel;
	reply += (" :" + topic);
	reply += "\r\n";
	return reply;
}

std::string ReplyBuilder::mode (const Client &opUser, const std::string &channel,
								const std::string &modeStr) {
	std::string reply = buildPrefix (opUser);
	reply += " MODE ";
	reply += channel;
	reply += (" " + modeStr);
	reply += "\r\n";
	return reply;
}

// Numeric Reply messages
const std::string ReplyBuilder::MSG_WELCOME = ":Welcome to the ircServ"; 					 // 001
const std::string ReplyBuilder::MSG_YOURHOST = ":Your host is ircServ, running version 1.0"; // 002
const std::string ReplyBuilder::MSG_CREATED = ":This server was created 2026-??-??"; 		 // 003
const std::string ReplyBuilder::MSG_MYINFO = "ircServ 1.0"; 								 // 004
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
	std::string nick = target.getNickName ();
	if (nick.empty ())
		nick = "*";
	return nick;
}

std::string ReplyBuilder::getNumericMessage (const std::string &num) {
	if (num == "001")
		return ReplyBuilder::MSG_WELCOME;
	else if (num == "002")
		return ReplyBuilder::MSG_YOURHOST;
	else if (num == "003")
		return ReplyBuilder::MSG_CREATED;
	else if (num == "004")
		return ReplyBuilder::MSG_MYINFO;
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
	std::string nick = client.getNickName ();
	if (nick.empty ())
		nick = "*";
	std::string user = client.getUserName ();
	if (user.empty ())
		user = "unknown";
	std::string prefix = ":" + nick + "!" + user + "@ircServ";
	return prefix;
}
