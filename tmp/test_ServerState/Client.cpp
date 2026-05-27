/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/23 19:45:12 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/24 14:16:15 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client(int socketFd)
	: _socketFd(socketFd), _username(""), _nickname(""), _realname(""),
	  _passAccepted(false), _registered(false)
{
}

Client::~Client()
{
}

int					Client::getSocketFd() const
{
	return this->_socketFd;
}

bool				Client::isPassAccepted() const
{
	return this->_passAccepted;
}

void				Client::acceptPassword()
{
	this->_passAccepted = true;
	return ;
}

const std::string&	Client::getUserName() const
{
	return this->_username;
}

void				Client::setUserName(const std::string& name)
{
	this->_username = name;
	return ;
}

const std::string&	Client::getNickName() const
{
	return this->_nickname;
}

void				Client::setNickByServerState(const std::string& name)
{
	this->_nickname = name;
	return ;
}

bool				Client::isRegistered() const
{
	return this->_registered;
}

bool				Client::tryRegister()
{
	if (isRegistered())
		return true;
	if (!this->isPassAccepted() || this->_username == "" || this->_nickname == "")
		return false;
	this->setAsRegistered();
	return true;
}

const std::string&	Client::getRealName() const
{
	return this->_realname;
}

void				Client::setRealName(const std::string& name)
{
	this->_realname = name;
	return ;
}

void				Client::setAsRegistered()
{
	this->_registered = true;
	return ;
}
//Forbidden OCF Functions
//
//Client::Client()
//{
//}
//
//Client::Client(const Client& src)
//{
//}
//
//Client&				Client::operator=(const Client& src)
//{
//	this->_socketFd = src._socketFd;
//	this->_username = src._username;
//	this->_nickname = src._nickname;
//	this->_realname = src._realname;
//	this->_passAccepted = src._passAccepted;
//	this->_registered = src._registered;
//	return *this;
//}
