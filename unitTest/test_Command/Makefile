NAME		= test_Command
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic -I$(INC_DIR)

INC_DIR		= ../../includes/
SRCS_DIR	= ../../src/
SRCS		= test_Command.cpp \
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

OBJS		= $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)


re: fclean all

.PHONY: all clean fclean re
