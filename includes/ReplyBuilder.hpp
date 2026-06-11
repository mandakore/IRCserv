#pragma once
#include <string>

class Client;

class ReplyBuilder {
public:
	// Functions
	// General Numeric Replies
	static std::string numeric (const std::string &target, const std::string &num,
								const std::string &msg);
	static std::string numeric (const Client &client, const std::string &num,
								const std::string &msg);

	// User Commands
	static std::string join (const Client &client, const std::string &channel);
	static std::string privMsg (const Client &from, const std::string &to, const std::string &msg);

	// Operator Commands
	static std::string kick (const Client &opUser, const Client &target, const std::string &channel,
							 const std::string &reason);
	static std::string invite (const Client &opUser, const Client &target,
							   const std::string &channel);
	static std::string topic (const Client &opUser, const std::string &channel,
							  const std::string &topic);
	static std::string mode (const Client &opUser, const std::string &channel,
							 const std::string &modeStr);

private:
	// Numeric Messages Definition // Format: Code / arg:"Required param puttern <Optional param>"
	// Numeric Reply Messages
	static const std::string MSG_WELCOME;		// 001 arg:""
	static const std::string MSG_YOURHOST;		// 002 arg:""
	static const std::string MSG_CREATED;		// 003 arg:""
	static const std::string MSG_MYINFO;		// 004 arg:""
	static const std::string MSG_CHANNELMODEIS; // 324 arg:"channel mode <modeParams>"
	static const std::string MSG_NOTOPIC;		// 331 arg:"channel"
	static const std::string MSG_TOPIC;			// 332 arg:"channel :topic"
	static const std::string MSG_INVITING;		// 341 arg:"nick channel"
	static const std::string MSG_NAMREPLY;		// 353 arg:"channel :nick <nick2...>"
	static const std::string MSG_ENDOFNAMES;	// 366 arg:"channel"

	// Numeric Error Messages
	static const std::string MSG_NOSUCHNICK;		// 401 arg:"nick"
	static const std::string MSG_NOSUCHCHANNEL;		// 403 arg:"channel"
	static const std::string MSG_CANNOTSENDTOCHAN;	// 404 arg:"channel"
	static const std::string MSG_NORECIPIENT;		// 411 arg:"<command>"
	static const std::string MSG_NOTEXTTOSEND;		// 412 arg:""
	static const std::string MSG_UNKNOWNCOMMAND;	// 421 arg:"command"
	static const std::string MSG_NONICKNAMEGIVEN;	// 431 arg:""
	static const std::string MSG_ERRONEUSNICKNAME;	// 432 arg:"nick"
	static const std::string MSG_NICKNAMEINUSE;		// 433 arg:"nick"
	static const std::string MSG_USERNOTINCHANNEL;	// 441 arg:"nick channel"
	static const std::string MSG_NOTONCHANNEL;		// 442 arg:"channel"
	static const std::string MSG_USERONCHANNEL;		// 443 arg:"nick channel"
	static const std::string MSG_NOTREGISTERED;		// 451 arg:"command"
	static const std::string MSG_NEEDMOREPARAMS;	// 461 arg:"command"
	static const std::string MSG_ALREADYREGISTERED; // 462 arg:""
	static const std::string MSG_PASSWDMISMATCH;	// 464 arg:""
	static const std::string MSG_KEYSET;			// 467 arg:"channel"
	static const std::string MSG_CHANNELISFULL;		// 471 arg:"channel"
	static const std::string MSG_UNKNOWNMODE;		// 472 arg:"modeChar"
	static const std::string MSG_INVITEONLYCHAN;	// 473 arg:"channel"
	static const std::string MSG_BADCHANNELKEY;		// 475 arg:"channel"
	static const std::string MSG_CHANOPRIVSNEEDED;	// 482 arg:"channel"

	// Unknown Numeric Code
	static const std::string MSG_UNKNOWNNUMERIC;
	// Private helper
	static std::string getValidTargetName (const Client &target);
	static std::string getNumericMessage (const std::string &num);
	static std::string buildPrefix (const Client &client);

	// Forbidden OCF Functions
	ReplyBuilder ();
	ReplyBuilder (const ReplyBuilder &src);
	~ReplyBuilder ();
	ReplyBuilder &operator= (const ReplyBuilder &src);
};
