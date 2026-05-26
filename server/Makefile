NAME		:= ircserv
SRCS		:= main.cpp Client.cpp CommandParse.cpp
OBJS		:= $(SRCS:.cpp=.o)
CXX			:= c++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98
DEBUG		:= -pedantic -Wshadow -g2 -O0
SANITIZE	:= -fsanitize=address,undefined -fno-omit-frame-pointer

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(OBJS) -o $@ $(SANITIZE)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

run : all
	./$(NAME)

.PHONY : all clean fclean re run
