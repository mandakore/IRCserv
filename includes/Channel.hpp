/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:57:17 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/18 17:17:11 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <set>
# include <string>
# include <vector>
# include "ChannelModes.hpp"

class Client;

class Channel
{
	public :
		//Constrcutor&Destructor
		Channel();
		Channel(const Channel& src);
		Channel(const std::string& name);
		~Channel();

		//Overlode Operator
		Channel&				operator=(const Channel& src);

		//Functions
		//Getter
		const std::string&		getChannelName() const;
		const std::string&		getChannelTopic() const;
		std::vector<Client*>	getMembers() const;
		std::string				getChannelModes() const;

		//Setter
		void					setChannelTopic(const std::string& topic);
		bool					applyMode(char opt, bool flag, const std::string& val);
		bool					addMember(Client* target);
		bool					addOperator(Client* target);
		bool					addInvitedMember(Client* target);

		//UnSetter
		bool					removeMember(Client* target);
		bool					removeOperator(Client* target);
		bool					removeInvitedMember(Client* target);

		//Util Functions
		bool					isChannelMember(Client* target) const;
		bool					isOperator(Client* target) const;
		bool					isInvitedMember(Client* target) const;

	private :
		std::string			_name;
		std::set<Client*>	_members;
		std::set<Client*>	_operators;
		std::set<Client*>	_invited;
		std::string			_topic;
		ChannelModes		_modes;
};

