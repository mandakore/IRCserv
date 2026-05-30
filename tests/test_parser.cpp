#include "Parser.hpp"
#include "Message.hpp"
#include "Debug.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

static int g_pass = 0;
static int g_fail = 0;

static void run_test(void (*test_func)(), const char *test_name) {
	try {
		test_func();
		std::cout << GREEN << "  [PASS] " << RESET << test_name << std::endl;
		++g_pass;
	} catch (const std::exception &e) {
		std::cout << RED << "  [FAIL] " << RESET << test_name << " : " << e.what() << std::endl;
		++g_fail;
	}
}

// 値の一致
template <typename T, typename U>
inline void expect_eq_helper(const T &actual, const U &expected, int line) {
	if (actual != expected) {
		std::ostringstream oss;
		oss << "Expected [" << expected << "] but got [" << actual << "]"
		    << " at line " << line;
		throw std::runtime_error(oss.str());
	}
}

// bool
inline void expect_true_helper(bool condition, const char *cond_str, int line) {
	if (!condition) {
		std::ostringstream oss;
		oss << "Condition is false: " << cond_str << " at line " << line;
		throw std::runtime_error(oss.str());
	}
}

inline void expect_false_helper(bool condition, const char *cond_str, int line) {
	if (condition) {
		std::ostringstream oss;
		oss << "Condition is true (expected false): " << cond_str << " at line " << line;
		throw std::runtime_error(oss.str());
	}
}

#define TEST(name) static void name ()

#define RUN(name) run_test(name, #name)

#define EXPECT_EQ(a, b) expect_eq_helper((a), (b), __LINE__)
#define EXPECT_TRUE(cond) expect_true_helper((cond), #cond, __LINE__)
#define EXPECT_FALSE(cond) expect_false_helper((cond), #cond, __LINE__)

// NICK基本
TEST (test_nick_basic) {
	Message msg = Parser::parse ("NICK hoge");
	EXPECT_EQ (msg.getCommand (), "NICK");
	EXPECT_EQ (msg.getParamCount (), (size_t)1);
	EXPECT_EQ (msg.getSingleParam (0), "hoge");
}

// 小文字NICK
TEST (test_nick_lowercase_normalized) {
	Message msg = Parser::parse ("nick hoge2");
	EXPECT_EQ (msg.getCommand (), "NICK");
	EXPECT_EQ (msg.getSingleParam (0), "hoge2");
}

// 大文字小文字NICK
TEST (test_nick_mixed_case_normalized) {
	Message msg = Parser::parse ("NiCk Bob");
	EXPECT_EQ (msg.getCommand (), "NICK");
	EXPECT_EQ (msg.getSingleParam (0), "Bob");
}

// USER
TEST (test_user_command) {
	Message msg = Parser::parse ("USER hoge 0 * :Real Name");
	EXPECT_EQ (msg.getCommand (), "USER");
	EXPECT_EQ (msg.getParamCount (), (size_t)4);
	EXPECT_EQ (msg.getSingleParam (0), "hoge");
	EXPECT_EQ (msg.getSingleParam (1), "0");
	EXPECT_EQ (msg.getSingleParam (2), "*");
	EXPECT_EQ (msg.getSingleParam (3), "Real Name");
}

// PASS
TEST (test_pass_command) {
	Message msg = Parser::parse ("PASS secretpass");
	EXPECT_EQ (msg.getCommand (), "PASS");
	EXPECT_EQ (msg.getParamCount (), (size_t)1);
	EXPECT_EQ (msg.getSingleParam (0), "secretpass");
}

// JOIN
TEST (test_join_command) {
	Message msg = Parser::parse ("JOIN #general");
	EXPECT_EQ (msg.getCommand (), "JOIN");
	EXPECT_EQ (msg.getParamCount (), (size_t)1);
	EXPECT_EQ (msg.getSingleParam (0), "#general");
}

// JOIN鍵付き
TEST (test_join_with_key) {
	Message msg = Parser::parse ("JOIN #secret mykey");
	EXPECT_EQ (msg.getCommand (), "JOIN");
	EXPECT_EQ (msg.getParamCount (), (size_t)2);
	EXPECT_EQ (msg.getSingleParam (0), "#secret");
	EXPECT_EQ (msg.getSingleParam (1), "mykey");
}

// QUIT
TEST (test_quit_no_params) {
	Message msg = Parser::parse ("QUIT");
	EXPECT_EQ (msg.getCommand (), "QUIT");
	EXPECT_EQ (msg.getParamCount (), (size_t)0);
}

// QUITメッセージ付き
TEST (test_quit_with_reason) {
	Message msg = Parser::parse ("QUIT :Leaving for now");
	EXPECT_EQ (msg.getCommand (), "QUIT");
	EXPECT_EQ (msg.getParamCount (), (size_t)1);
	EXPECT_EQ (msg.getSingleParam (0), "Leaving for now");
}

// PRIVMSG（trailing）

// チャンネルメッセージ
TEST (test_privmsg_to_channel) {
	Message msg = Parser::parse ("PRIVMSG #room :hello world");
	EXPECT_EQ (msg.getCommand (), "PRIVMSG");
	EXPECT_EQ (msg.getParamCount (), (size_t)2);
	EXPECT_EQ (msg.getSingleParam (0), "#room");
	EXPECT_EQ (msg.getSingleParam (1), "hello world");
}

// ユーザーメッセージ
TEST (test_privmsg_to_user) {
	Message msg = Parser::parse ("PRIVMSG hoge :hey there!");
	EXPECT_EQ (msg.getCommand (), "PRIVMSG");
	EXPECT_EQ (msg.getSingleParam (0), "hoge");
	EXPECT_EQ (msg.getSingleParam (1), "hey there!");
}

// messageの中にコロン
TEST (test_privmsg_trailing_colon_in_text) {
	Message msg = Parser::parse ("PRIVMSG #ch :hello :world: test");
	EXPECT_EQ (msg.getCommand (), "PRIVMSG");
	EXPECT_EQ (msg.getSingleParam (1), "hello :world: test");
}

// massageの中にスペース
TEST (test_privmsg_trailing_spaces_preserved) {
	Message msg = Parser::parse ("PRIVMSG #ch :  spaces  ");
	EXPECT_EQ (msg.getSingleParam (1), "  spaces  ");
}

// refix のスキップ

TEST (test_prefix_skipped) {
	Message msg = Parser::parse (":nick!user@host PRIVMSG #ch :hi");
	EXPECT_EQ (msg.getCommand (), "PRIVMSG");
	EXPECT_EQ (msg.getSingleParam (0), "#ch");
	EXPECT_EQ (msg.getSingleParam (1), "hi");
}

TEST (test_prefix_servername_skipped) {
	Message msg = Parser::parse (":irc.example.com 001 hoge :Welcome");
	EXPECT_EQ (msg.getCommand (), "001");
	EXPECT_EQ (msg.getSingleParam (0), "hoge");
	EXPECT_EQ (msg.getSingleParam (1), "Welcome");
}

// 空行・空白 
TEST (test_empty_line) {
	Message msg = Parser::parse ("");
	EXPECT_EQ (msg.getCommand (), "");
	EXPECT_EQ (msg.getParamCount (), (size_t)0);
}

TEST (test_whitespace_only_line) {
	Message msg = Parser::parse ("   ");
	EXPECT_EQ (msg.getCommand (), "");
}

// hasParam
TEST (test_has_param_true) {
	Message msg = Parser::parse ("NICK hoge");
	EXPECT_TRUE (msg.hasParam (0));
	EXPECT_FALSE (msg.hasParam (1));
}

TEST (test_has_param_false_on_no_params) {
	Message msg = Parser::parse ("QUIT");
	EXPECT_FALSE (msg.hasParam (0));
}

TEST (test_get_single_param_out_of_range_returns_empty) {
	Message msg = Parser::parse ("NICK hoge");
	EXPECT_EQ (msg.getSingleParam (5), "");
}

// ── MODE コマンド（複合パラメータ）

TEST (test_mode_channel_flag) {
	Message msg = Parser::parse ("MODE #room +i");
	EXPECT_EQ (msg.getCommand (), "MODE");
	EXPECT_EQ (msg.getSingleParam (0), "#room");
	EXPECT_EQ (msg.getSingleParam (1), "+i");
}

TEST (test_mode_with_argument) {
	Message msg = Parser::parse ("MODE #room +k password");
	EXPECT_EQ (msg.getCommand (), "MODE");
	EXPECT_EQ (msg.getSingleParam (0), "#room");
	EXPECT_EQ (msg.getSingleParam (1), "+k");
	EXPECT_EQ (msg.getSingleParam (2), "password");
}

TEST (test_mode_user) {
	Message msg = Parser::parse ("MODE hoge +o");
	EXPECT_EQ (msg.getCommand (), "MODE");
	EXPECT_EQ (msg.getSingleParam (0), "hoge");
	EXPECT_EQ (msg.getSingleParam (1), "+o");
}

// KICK / INVITE / TOPIC -------------

TEST (test_kick_command) {
	Message msg = Parser::parse ("KICK #room baduser :Get away!");
	EXPECT_EQ (msg.getCommand (), "KICK");
	EXPECT_EQ (msg.getSingleParam (0), "#room");
	EXPECT_EQ (msg.getSingleParam (1), "baduser");
	EXPECT_EQ (msg.getSingleParam (2), "Get away!");
}

TEST (test_invite_command) {
	Message msg = Parser::parse ("INVITE hoge #room");
	EXPECT_EQ (msg.getCommand (), "INVITE");
	EXPECT_EQ (msg.getSingleParam (0), "hoge");
	EXPECT_EQ (msg.getSingleParam (1), "#room");
}

TEST (test_topic_set) {
	Message msg = Parser::parse ("TOPIC #room :New topic here");
	EXPECT_EQ (msg.getCommand (), "TOPIC");
	EXPECT_EQ (msg.getSingleParam (0), "#room");
	EXPECT_EQ (msg.getSingleParam (1), "New topic here");
}

TEST (test_topic_query) {
	Message msg = Parser::parse ("TOPIC #room");
	EXPECT_EQ (msg.getCommand (), "TOPIC");
	EXPECT_EQ (msg.getParamCount (), (size_t)1);
	EXPECT_EQ (msg.getSingleParam (0), "#room");
	EXPECT_FALSE (msg.hasParam (1));
}

// 複数スペース

TEST (test_multiple_spaces_between_params) {
	Message msg = Parser::parse ("NICK  hoge");
	EXPECT_EQ (msg.getCommand (), "NICK");
	EXPECT_EQ (msg.getSingleParam (0), "hoge");
}

// RFC最大パラメータ数

TEST (test_max_params_limit) {
	Message msg = Parser::parse ("CMD p1 p2 p3 p4 p5 p6 p7 p8 p9 p10 p11 p12 p13 p14 p15 p16 p17");
	EXPECT_EQ (msg.getCommand (), "CMD");
	EXPECT_TRUE (msg.getParamCount () <= 15);
}

// Message コンストラクタ直接テスト 

// TEST (test_message_direct_construction) {
// 	std::vector<std::string> params;
// 	params.push_back ("hoge");
// 	params.push_back ("0");
// 	params.push_back ("*");
// 	params.push_back ("Real Name");
// 	Message msg ("USER", params);
// 	EXPECT_EQ (msg.getCommand (), "USER");
// 	EXPECT_EQ (msg.getParamCount (), (size_t)4);
// 	EXPECT_EQ (msg.getSingleParam (3), "Real Name");
// }

// TEST (test_message_copy_constructor) {
// 	Message original = Parser::parse ("NICK hoge");
// 	Message copy (original);
// 	EXPECT_EQ (copy.getCommand (), "NICK");
// 	EXPECT_EQ (copy.getSingleParam (0), "hoge");
// }

// TEST (test_message_assignment_operator) {
// 	Message msg1 = Parser::parse ("NICK hoge");
// 	Message msg2 = Parser::parse ("QUIT");
// 	msg2 = msg1;
// 	EXPECT_EQ (msg2.getCommand (), "NICK");
// 	EXPECT_EQ (msg2.getSingleParam (0), "hoge");
// }

// main 

int main () {
	std::cout << "=== Parser Unit Tests ===" << std::endl;
	std::cout << std::endl;

	std::cout << "[Group: Basic Commands]" << std::endl;
	RUN (test_nick_basic);
	RUN (test_nick_lowercase_normalized);
	RUN (test_nick_mixed_case_normalized);
	RUN (test_user_command);
	RUN (test_pass_command);
	RUN (test_join_command);
	RUN (test_join_with_key);
	RUN (test_quit_no_params);
	RUN (test_quit_with_reason);

	std::cout << std::endl;
	std::cout << "[Group: PRIVMSG (trailing)]" << std::endl;
	RUN (test_privmsg_to_channel);
	RUN (test_privmsg_to_user);
	RUN (test_privmsg_trailing_colon_in_text);
	RUN (test_privmsg_trailing_spaces_preserved);

	std::cout << std::endl;
	std::cout << "[Group: Prefix skip]" << std::endl;
	RUN (test_prefix_skipped);
	RUN (test_prefix_servername_skipped);

	std::cout << std::endl;
	std::cout << "[Group: Empty / Whitespace]" << std::endl;
	RUN (test_empty_line);
	RUN (test_whitespace_only_line);

	std::cout << std::endl;
	std::cout << "[Group: hasParam / getSingleParam]" << std::endl;
	RUN (test_has_param_true);
	RUN (test_has_param_false_on_no_params);
	RUN (test_get_single_param_out_of_range_returns_empty);

	std::cout << std::endl;
	std::cout << "[Group: MODE / KICK / INVITE / TOPIC]" << std::endl;
	RUN (test_mode_channel_flag);
	RUN (test_mode_with_argument);
	RUN (test_mode_user);
	RUN (test_kick_command);
	RUN (test_invite_command);
	RUN (test_topic_set);
	RUN (test_topic_query);

	std::cout << std::endl;
	std::cout << "[Group: Edge Cases]" << std::endl;
	RUN (test_multiple_spaces_between_params);
	RUN (test_max_params_limit);

	std::cout << std::endl;
	std::cout << "[Group: Message Class]" << std::endl;
	// RUN (test_message_direct_construction);
	// RUN (test_message_copy_constructor);
	// RUN (test_message_assignment_operator);

	std::cout << std::endl;
	std::cout << "=========================" << std::endl;
	std::cout << "Results: " << g_pass << " passed, " << g_fail << " failed." << std::endl;

	return (g_fail > 0) ? 1 : 0;
}
