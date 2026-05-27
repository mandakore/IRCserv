/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_ServerState.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sohyamaz <sohyamaz@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 12:16:25 by sohyamaz          #+#    #+#             */
/*   Updated: 2026/05/27 14:16:02 by sohyamaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "Channel.hpp"
#include "ChannelModes.hpp"
#include "Client.hpp"
#include "ServerState.hpp"

static void	expect(bool result, const std::string testCase)
{
	if (result)
		std::cout << "[OK]" << testCase << std::endl;
	else
		std::cout << "[KO]" << testCase << std::endl;
}

int	main()
{
	ServerState state("pass");

	expect(state.isCorrectPassword("pass") == true, "Correct Password");
	expect(state.isCorrectPassword("hoge") == false, "Incorrect Password");
	expect(state.addNewClient(10) == true, "add client as socketFd=10");
	expect(state.getClientByFd(10) != NULL, "get client by socketFd=10");
	expect(state.addNewClient(10) == false, "duplicate fd should rejected");
	Client*	socket10 = state.getClientByFd(10);
	expect(socket10 != NULL, "Client ptr is not NULL");
	expect(state.updateNickname(*socket10, "SHATTORI") == true, "set nick SHATTORI");
	expect(state.isNicknameExist("SHATTORI") == true, "nick SHATTORI exists");
	expect(state.getClientByNick("SHATTORI") == socket10, "get client by nick SHATTORI");
	expect(state.ensureChannel("test") != NULL, "Channel Create");
	expect(state.findChannel("test") != NULL, "Channel Search");
	expect(state.removeChannelIfEmpty("test") == true, "remove empty Channel");
	expect(state.updateNickname(*socket10, "ATASHIRO") == true, "update nickname");
	expect(state.getClientByNick("SHATTORI") == NULL, "Old nickname is not hit");
	expect(state.getClientByNick("ATASHIRO") == socket10, "new Nickname should hit");
	expect(state.isNicknameExist("SHATTORI") == false, "Old nickname should deleted");
	expect(state.isNicknameExist("ATASHIRO") == true, "New nickname should exist");
	state.addNewClient(20);
	Client*	socket20 = state.getClientByFd(20);
	expect(state.updateNickname(*socket20, "ATASHIRO") == false, "nickname must be unique");
	expect(state.updateNickname(*socket20, "TOYAMAGU") == true, "Unique nick is OK");
	Channel*	ch1 = state.ensureChannel("2ch");
	Channel*	ch2 = state.ensureChannel("2ch");
	expect(ch1 = ch2, "same channelname will not create");
	ch1->addMember(socket10);
	expect(state.removeChannelIfEmpty("2ch") == false, "Do not remove non-Empty channel");
	ch1->addOperator(socket10);
	ch1->addInvitedMember(socket10);
	expect(state.removeClient(10) == true, "Client remove");
	expect(state.getClientByFd(10) == NULL, "deleted client should not hit by fd");
	expect(state.getClientByNick("ATASHIRO") == NULL, "deleted client should not hit by nick");
	expect(state.findChannel("2ch") == NULL, "empty channel will delete automatically");
	expect(state.removeClient(42) == false, "Unexist fd should not remove");
	expect(state.removeChannelIfEmpty("hogefuga") == false, "non-exist channel should not remove");
	expect(state.getClientByFd(42) == NULL, "non-exist fd should not hit");
	expect(state.getClientByNick("ZAKI") == NULL, "non-exist nick should not hit");
	expect(state.findChannel("5ch") == NULL, "non-exist channel should not hit");
	expect(state.updateNickname(*socket20, "TOYAMAGU") == false, "Same nick is not allowed");
	expect(state.updateNickname(*socket20, "") == false, "empty nick is not allowed");
	state.addNewClient(30);
	Client*	socket30 = state.getClientByFd(30);
	state.updateNickname(*socket30, "ENKWAK");
	Channel*	ch3 = state.ensureChannel("5ch");
	ch3->addMember(socket30);
	ch3->addMember(socket20);
	ch3->addOperator(socket30);
	ch3->addInvitedMember(socket30);
	expect(ch3->getMemberCount() == 2, "2 users should registered");
	state.removeClient(20);
	expect(state.removeChannelIfEmpty("5ch") == false, "non-empty channel should not delete");
	return 0;
}
