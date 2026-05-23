/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:45:56 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/23 19:06:21 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>
# include "Message.hpp"

class Parser
{
	public :
		//Constructor&Destructor
		Parser();
		Parser(const Parser& src);
		~Parser();

		//Overload Operator
		Parser&			operator=(const Parser& src);

		//Functions
		static Message	parse(const std::string& line);
	private :
};

