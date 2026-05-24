/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerState.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 14:48:23 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/24 17:28:19 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerState.hpp"

ServerState::ServerState(const std::string& password)
	: _password(password)
{
}

ServerState::~ServerState()
{
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
