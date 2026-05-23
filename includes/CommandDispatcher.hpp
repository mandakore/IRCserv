/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandDispatcher.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:47:37 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/16 21:48:12 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

class CommandDispatcher
{
	public :
		CommandDispatcher();
		CommandDispatcher(const CommandDispatcher& src);
		~CommandDispatcher();
		CommandDispatcher&	operator=(const CommandDispatcher& src);
	private :

};

