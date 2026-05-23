/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReplyBuilder.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:46:42 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/23 16:21:04 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>

class Client

class ReplyBuilder
{
	public :
		//Constructor&Destructor
		ReplyBuilder();
		ReplyBuilder(const ReplyBuilder& src);
		~ReplyBuilder();

		//Overlode Operator
		ReplyBuilder&	operator=(const ReplyBuilder& src);

		//Functions
		//General Numeric Replies
		static std::string	numeric(const std::string& target,
									const std::string& num,
									const std::string& msg);
		static std::string	numeric(const Client& client,
									const std::string& num,
									const std::string& msg);

		//User Commands
		static std::string	join(const Client& client, const std::string& channel);
		static std::string	privMsg(const Client& from,
									const std::string& to,
									const std::string& msg);

		//Operator Commands
		static std::string	kick(const Client& opUser, const Client& target,
								 const std::string& channel, const std::string& reason);
		static std::string	invite(const Client& opUser,
								   const Client& target,
								   const std::string& channel);
		static std::string	topic(const Client& opUser,
								  const std::string& channel,
								  const std::string& topic);
		static std::string	mode(const Client& opUser,
								  const std::string& channel,
								  const std::string& modeStr);

	private :
};

