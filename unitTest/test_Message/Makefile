NAME		= test_Message
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic -I$(INC_DIR)

INC_DIR		= ../../includes/
SRCS_DIR	= ../../src/
SRCS		= test_Message.cpp \
			  $(SRCS_DIR)Message.cpp \

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
