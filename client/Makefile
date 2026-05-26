NAME		= client
SRCS		= client.cpp
OBJS		= $(SRCS:.cpp=.o)
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(OBJS) -o $@

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -f $(NAME)

re : fclean all

run : all
	./$(NAME)

.PHONY : all clean fclean re run
