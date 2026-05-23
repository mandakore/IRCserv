/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelModes.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:55:50 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/17 19:24:26 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>

class ChannelModes
{
	public :
		//Constructor&Destructor
		ChannelModes();
		ChannelModes(const ChannelModes& src);
		~ChannelModes();

		//Overlode Operator
		ChannelModes&	operator=(const ChannelModes& src);

		//Functions
		//Getters
		bool			isInviteOnly() const;
		bool			isTopicRestricted() const;
		bool			isMemberLimited() const;
		bool			isChannelProtected() const;
		int				getMemberLimit() const;
		bool			checkChannelPassword(const std::string& input) const;

		//Setters&Unsetter
		void			setInviteOnly();
		void			unsetInviteOnly();
		void			setTopicRestricted();
		void			unsetTopicRestricted();
		void			setMemberLimit(int limit);
		void			unsetMemberLimit();
		void			setChannelProtected(const std::string& password);
		void			unsetChannelProtected();
	private :
		bool			_inviteOnly;
		bool			_topicRestricted;
		bool			_memberLimited;
		bool			_channelProtected;
		int				_maxMember;
		std::string		_channelPass;
};

