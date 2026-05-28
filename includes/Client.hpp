#pragma once
#include <string>

class Client {
public:
	// Constructor&Destructor
	Client (int socketFd);
	~Client ();

	// Functions
	// SocketFd
	int getSocketFd () const;

	// Registration Functions
	// Password
	bool isPassAccepted () const;
	void acceptPassword ();

	// Username
	const std::string &getUserName () const;
	void setUserName (const std::string &name);

	// Nickname
	const std::string &getNickName () const;
	void setNickByServerState (const std::string &name);

	// Register
	bool isRegistered () const;
	bool tryRegister ();

	// Optional Realname
	const std::string &getRealName () const;
	void setRealName (const std::string &name);

private:
	// Variables
	int _socketFd;
	std::string _username;
	std::string _nickname;
	std::string _realname;
	bool _passAccepted;
	bool _registered;

	// Helpers called only from public functions
	void setAsRegistered ();

	// Forbidden OCF Functions
	Client ();
	Client (const Client &src);
	Client &operator= (const Client &src);
};
