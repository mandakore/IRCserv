NAME		= ircserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic

SRCS_DIR	= src/
SRCS		= $(SRCS_DIR)main.cpp \
			  $(SRCS_DIR)Banner.cpp \
			  $(SRCS_DIR)Server.cpp 

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