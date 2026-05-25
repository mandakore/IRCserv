/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerState.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 14:48:23 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/25 21:02:05 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerState.hpp"

ServerState::ServerState(const std::string& password)
	: _password(password)
{
}

ServerState::~ServerState()
{
	this->_clientsByNick.clear();
	for (std::map<std::string, Channel*>::iterator itCh = _channels.begin();
			itCh != _channels.end(); ++itCh)
		delete itCh->second;
	this->_channels.clear();
	for (std::map<int, Client*>::iterator itCl = _clientsByFd.begin();
			itCl != _clientsByFd.end(); ++itCl)
		delete itCl->second;
	this->_clientsByFd.clear();
}

bool		ServerState::addNewClient(int socketFd)
{
}

bool		ServerState::removeClient(int socketFd)
{
}

bool		ServerState::updateNickname(Client& client, const std::string& nick)
{
}

bool		ServerState::isNicknameExist(const std::string& nick) const
{
}

Client*		ServerState::getClientByFd(int socketFd) const
{
}

Client*		ServerState::getClientByNick(const std::string& nick) const
{
}

Channel*	ServerState::ensureChannel(const std::string& name)
{
}

bool		ServerState::removeChannelIfEmpty(const std::string& name)
{
}

Channel*	ServerState::findChannel(const std::string& name) const
{
}

bool		ServerState::isCorrectPassword(const std::string& pass) const
{
}
