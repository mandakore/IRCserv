#pragma once

#include <string>
#include <vector>
#include "CommandResult.hpp"

class Channel;
class Client;
class Message;
class ServerState;

class CommandDispatcher {
public:
	// Functions
	static CommandResult dispatch (int fd, const Message &msg, ServerState &state);

private:
	// variables
	enum e_command {
		CMD_INVALID = 0,
		CMD_PASS,
		CMD_NICK,
		CMD_USER,
		CMD_JOIN,
		CMD_PRIVMSG,
		CMD_KICK,
		CMD_INVITE,
		CMD_TOPIC,
		CMD_MODE,
		CMD_PING,
		CMD_COUNT
	};

	static const std::string cmds[CMD_COUNT];

	// Handler helpers
	// general
	static int _cmdNameToNumber (const std::string cmds[], const std::string &cmd);
	static void _broadcastToChannel (CommandResult &result, const Channel &channel,
									 const std::string &message, const Client *except);
	static std::vector<std::string> _splitByComma (const std::string &target);
	// NICK
	static bool _isLetterSpecial (unsigned char c);
	static bool _isValidNick (const std::string &nick);
	// JOIN
	static bool _isValidChannelName (const std::string &name);
	static void _notifyTopic (int fd, const Client &client, const Channel &channel,
							  CommandResult &result);
	static void _notifyMembers (int fd, const Client &client, const Channel &channel,
								CommandResult &result);
	static void _joinSingleChannel (int fd, Client &client, const std::string &channel,
									const std::string &key, ServerState &state,
									CommandResult &result);
	// PRIVMSG
	static void _sendPrivMsgToTarget (int fd, const Client &client, const std::string &target,
									  const std::string &msg, const ServerState &state,
									  CommandResult &result);
	// KICK
	static void _kickSingleTarget (int fd, Client &client, const std::string &channelName,
								   const std::string &nick, const std::string &reason,
								   ServerState &state, CommandResult &result);
	// Command handleress
	static CommandResult _handleInvalidCommand (int fd, const Message &msg, ServerState &state);
	static CommandResult _handlePass (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleNick (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleUser (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleJoin (int fd, const Message &msg, ServerState &state);
	static CommandResult _handlePrivMsg (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleKick (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleInvite (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleTopic (int fd, const Message &msg, ServerState &state);
	static CommandResult _handleMode (int fd, const Message &msg, ServerState &state);
	static CommandResult _handlePing (int fd, const Message &msg, ServerState &state);

	// Forbidden OCF Functions
	CommandDispatcher ();
	CommandDispatcher (const CommandDispatcher &src);
	~CommandDispatcher ();
	CommandDispatcher &operator= (const CommandDispatcher &src);
};
