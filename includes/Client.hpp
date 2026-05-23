/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:20:15 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/17 18:30:25 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>

class Client
{
	public :
		//Constructor&Destructor
		Client();
		Client(const Client& src);
		Client(int socketFd);
		~Client();

		//Overlode Operator
		Client&	operator=(const Client& src);

		//Functions
		int					getSocketFd() const;
		const std::string&	getUserName() const;
		const std::string&	getRealName() const;
		const std::string&	getNickName() const;
		bool				isPassAccepted() const;
		bool				isRegistered() const;
		void				setUserName(const std::string& name);
		void				setRealName(const std::string& name);
		void				setNickByServerState(const std::string& name);
		void				acceptPassword();
		bool				tryRegister();

	private :
		//variables
		int				_socketFd;
		std::string		_username;
		std::string		_realname;
		std::string		_nickname;
		bool			_passAccepted;
		bool			_registered;

		//Functions
		void				setAsRegistered();
};

