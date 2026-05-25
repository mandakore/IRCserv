/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandResult.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:33:40 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/23 19:05:58 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <vector>

struct	t_reply
{
	int			fd;
	std::string	reply;
};

class CommandResult
{
	public :
		//Constructor&Destructor
		CommandResult();
		CommandResult(const CommandResult& src);
		~CommandResult();

		//Overload Operator
		CommandResult&				operator=(const CommandResult& src);

		//Functions
		void						addReply(int fd, const std::string& reply);
		const std::vector<t_reply>&	getReplies() const;
		void						requestToDisconnect();
		bool						shouldDisconnect() const;

	private :
		std::vector<t_reply>	_replies;
		bool				_disconnect;
};

