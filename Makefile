NAME		= ircserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic -I$(INC_DIR)

INC_DIR		= includes/
SRCS_DIR	= src/
SRCS		= $(SRCS_DIR)main.cpp \
			  $(SRCS_DIR)Banner.cpp \
			  $(SRCS_DIR)Server.cpp \
			  $(SRCS_DIR)Channel.cpp \
			  $(SRCS_DIR)ChannelModes.cpp \
			  $(SRCS_DIR)Client.cpp \
			  $(SRCS_DIR)CommandDispatcher.cpp \
			  $(SRCS_DIR)CommandResult.cpp \
			  $(SRCS_DIR)Message.cpp \
			  $(SRCS_DIR)Parser.cpp \
			  $(SRCS_DIR)ReplyBuilder.cpp \
			  $(SRCS_DIR)ServerState.cpp \
			  $(SRCS_DIR)handleInvite.cpp \
			  $(SRCS_DIR)handleJoin.cpp \
			  $(SRCS_DIR)handleKick.cpp \
			  $(SRCS_DIR)handleMode.cpp \
			  $(SRCS_DIR)handleNick.cpp \
			  $(SRCS_DIR)handlePass.cpp \
			  $(SRCS_DIR)handlePrivMsg.cpp \
			  $(SRCS_DIR)handleTopic.cpp \
			  $(SRCS_DIR)handleUser.cpp \
			  $(SRCS_DIR)handlePing.cpp \
			  $(SRCS_DIR)handleCap.cpp \

OBJS		= $(SRCS:.cpp=.o)


all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)


re: fclean all

TEST_PORT = 6667
TEST_PASS = pass

test: all
	@echo "Starting server for testing on port $(TEST_PORT)..."
# 	@./$(NAME) $(TEST_PORT) $(TEST_PASS) > /dev/null 2>&1 & echo $$! > server.pid
	@./$(NAME) $(TEST_PORT) $(TEST_PASS) > server.log 2>&1 & echo $$! > server.pid
	@sleep 1
	@echo "Running Python tests..."
	@python3 IRCservTester/test.py || (kill `cat server.pid` && rm server.pid && exit 1)
	@echo "Stopping server..."
	@kill `cat server.pid` && rm server.pid
	@echo "All tests passed successfully!"

.PHONY: all clean fclean re test
