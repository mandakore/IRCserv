/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerState.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/24 14:48:23 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/27 12:51:04 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <new>
#include "Channel.hpp"
#include "Client.hpp"
#include "ServerState.hpp"

typedef std::map<int, Client *> clientFdMap;
typedef std::map<std::string, Client *> clientNickMap;
typedef std::map<std::string, Channel *> channelMap;

ServerState::ServerState (const std::string &password) : _password (password) {
}

ServerState::~ServerState () {
	_clientsByNick.clear ();
	for (channelMap::iterator itCh = _channels.begin (); itCh != _channels.end (); ++itCh)
		delete itCh->second;
	_channels.clear ();
	for (clientFdMap::iterator itCl = _clientsByFd.begin (); itCl != _clientsByFd.end (); ++itCl)
		delete itCl->second;
	_clientsByFd.clear ();
}

bool ServerState::addNewClient (int socketFd) {
	if (_clientsByFd.find (socketFd) != _clientsByFd.end ())
		return false;
	Client *newClient = NULL;
	try {
		newClient = new Client (socketFd);
	} catch (const std::bad_alloc &) {
		return false;
	}
	std::pair<clientFdMap::iterator, bool> result;
	result = _clientsByFd.insert (std::pair<int, Client *> (socketFd, newClient));
	if (result.second == false)
		delete newClient;
	return result.second;
}

bool ServerState::removeClient (int socketFd) {
	clientFdMap::iterator itCl = _clientsByFd.find (socketFd);
	if (itCl == _clientsByFd.end ())
		return false;
	Client *target = itCl->second;
	channelMap::iterator itCh = _channels.begin ();
	std::vector<std::string> removeList;
	for (; itCh != _channels.end (); ++itCh) {
		itCh->second->removeClient (target);
		if (itCh->second->getMemberCount () == 0)
			removeList.push_back (itCh->second->getChannelName ());
	}
	for (size_t i = 0; i < removeList.size (); ++i)
		removeChannelIfEmpty (removeList[i]);
	if (target->getNickName () != "")
		_clientsByNick.erase (target->getNickName ());
	_clientsByFd.erase (itCl);
	delete target;
	return true;
}

bool ServerState::updateNickname (Client &client, const std::string &nick) {
	if (nick == "")
		return false;
	std::string oldNick = client.getNickName ();
	if (nick == oldNick)
		return false;
	if (isNicknameExist (nick) == true)
		return false;
	std::pair<clientNickMap::iterator, bool> result;
	result = _clientsByNick.insert (std::pair<std::string, Client *> (nick, &client));
	if (result.second == false)
		return false;
	else {
		if (oldNick != "")
			_clientsByNick.erase (oldNick);
		client.setNickByServerState (nick);
		return true;
	}
}

bool ServerState::isNicknameExist (const std::string &nick) const {
	clientNickMap::const_iterator it = _clientsByNick.find (nick);
	if (it == _clientsByNick.end ())
		return false;
	else
		return true;
}

Client *ServerState::getClientByFd (int socketFd) const {
	clientFdMap::const_iterator it = _clientsByFd.find (socketFd);
	if (it == _clientsByFd.end ())
		return NULL;
	else
		return it->second;
}

Client *ServerState::getClientByNick (const std::string &nick) const {
	clientNickMap::const_iterator it = _clientsByNick.find (nick);
	if (it == _clientsByNick.end ())
		return NULL;
	else
		return it->second;
}

Channel *ServerState::ensureChannel (const std::string &name) {
	Channel *newChan = NULL;
	channelMap::iterator it = _channels.find (name);
	if (it != _channels.end ())
		return it->second;
	else {
		try {
			newChan = new Channel (name);
		} catch (const std::bad_alloc &) {
			return NULL;
		}
		std::pair<channelMap::iterator, bool> result;
		result = _channels.insert (std::pair<std::string, Channel *> (name, newChan));
		if (result.second == false) {
			delete newChan;
			return NULL;
		}
		return result.first->second;
	}
}

bool ServerState::removeChannelIfEmpty (const std::string &name) {
	channelMap::iterator itCh = _channels.find (name);
	if (itCh == _channels.end ())
		return false;
	Channel *target = itCh->second;
	if (target->getMemberCount () != 0)
		return false;
	_channels.erase (itCh);
	delete target;
	return true;
}

Channel *ServerState::findChannel (const std::string &name) const {
	channelMap::const_iterator it = _channels.find (name);
	if (it == _channels.end ())
		return NULL;
	else
		return it->second;
}

bool ServerState::isCorrectPassword (const std::string &pass) const {
	if (pass == _password)
		return true;
	else
		return false;
}
