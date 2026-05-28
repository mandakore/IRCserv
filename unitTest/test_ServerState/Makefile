NAME		= test_ServerState
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic -I$(INC_DIR)

INC_DIR		= ../../includes/
SRCS_DIR	= ../../src/
SRCS		= test_ServerState.cpp \
			  $(SRCS_DIR)Channel.cpp \
			  $(SRCS_DIR)ChannelModes.cpp \
			  $(SRCS_DIR)Client.cpp \
			  $(SRCS_DIR)ServerState.cpp \

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
