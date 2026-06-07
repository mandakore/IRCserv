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
	// Numeric Reply Messages
	static const std::string MSG_WELCOME;		// 001 arg:"nick!user"
	static const std::string MSG_CHANNELMODEIS; // 324
	static const std::string MSG_NOTOPIC;		// 331
	static const std::string MSG_TOPIC;			// 332
	static const std::string MSG_INVITING;		// 341
	static const std::string MSG_NAMREPRY;		// 353
	static const std::string MSG_ENDOFNAMES;	// 366

	// Numeric Error Messages
	static const std::string MSG_NOSUCHNICK;		// 401
	static const std::string MSG_NOSUCHCHANNEL;		// 403
	static const std::string MSG_CANNOTSENDTOCHAN;	// 404
	static const std::string MSG_TOOMANYTARGETS;	// 407
	static const std::string MSG_NORECIPIENT;		// 411
	static const std::string MSG_NOTEXTTOSEND;		// 412
	static const std::string MSG_UNKNOWNCOMMAND;	// 421
	static const std::string MSG_NONICKNAMEGIVEN;	// 431
	static const std::string MSG_ERRONEUSNICKNAME;	// 432
	static const std::string MSG_NICKNAMEINUSE;		// 433
	static const std::string MSG_USERNOTINCHANNEL;	// 441
	static const std::string MSG_NOTONCHANNEL;		// 442
	static const std::string MSG_USEONCHANNEL;		// 443
	static const std::string MSG_NEEDMOREPARAMS;	// 461
	static const std::string MSG_ALREADYREGISTERED; // 462
	static const std::string MSG_PASSWDMISMATCH;	// 464
	static const std::string MSG_KEYSET;			// 467
	static const std::string MSG_CHANNELISFULL;		// 471
	static const std::string MSG_UNKNOWNMODE;		// 472
	static const std::string MSG_INVITEONLYCHAN;	// 473
	static const std::string MSG_BADCHANNELKEY;		// 475
	static const std::string MSG_CHANOPRIVSNEEDED;	// 482

	static std::string getNumericMessage (const std::string &num);
	static std::string getTargetName (const Client &client);

	// Forbidden OCF Functions
	ReplyBuilder ();
	ReplyBuilder (const ReplyBuilder &src);
	~ReplyBuilder ();
	ReplyBuilder &operator= (const ReplyBuilder &src);
};
