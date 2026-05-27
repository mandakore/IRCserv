/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerState.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:48:43 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/24 14:56:24 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <map>
# include <string>

class Channel;
class Client;

class ServerState
{
	public :
		//Constructor&Destructor
		ServerState(const std::string& password);
		~ServerState();

		//Functions
		//Client
		bool			addNewClient(int socketFd);
		bool			removeClient(int socketFd);
		bool			updateNickname(Client& client, const std::string& nick);
		bool			isNicknameExist(const std::string& nick) const;
		Client*			getClientByFd(int socketFd) const;
		Client*			getClientByNick(const std::string& nick) const;

		//Channel
		Channel*		ensureChannel(const std::string& name);
		bool			removeChannelIfEmpty(const std::string& name);
		Channel*		findChannel(const std::string& name) const;

		//Server
		bool			isCorrectPassword(const std::string& pass) const;

	private :
		//Variables
		std::string						_password;
		std::map<int, Client*>			_clientsByFd;
		std::map<std::string, Client*>	_clientsByNick;
		std::map<std::string, Channel*>	_channels;

		//Forbidden OCF Functions
		ServerState();
		ServerState(const ServerState& src);
		ServerState&	operator=(const ServerState& src);
};

