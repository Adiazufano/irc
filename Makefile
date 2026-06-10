NAME        := ircserv

SRC_DIR     := src
OBJ_DIR     := obj
INC_DIR     := include
DEP_DIR     := deps

SRCS        := main.cpp Client.cpp CommandParse.cpp Server.cpp Channel.cpp \
				commands/cap.cpp commands/join.cpp commands/nick.cpp commands/pass.cpp commands/topic.cpp commands/user.cpp \
				commands/privmsg.cpp commands/kick.cpp commands/part.cpp commands/names.cpp\
				commands/invite.cpp \
				commands/ping.cpp
SRCS        := $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS        := $(SRCS:$(SRC_DIR)/%.cpp=$(DEP_DIR)/%.d)

CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR)
DEBUG       := -pedantic -Wshadow -g2 -O0
SANITIZE    := -fsanitize=address,undefined -fno-omit-frame-pointer

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(SANITIZE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	mkdir -p $(dir $@) $(DEP_DIR)/$(dir $*)
	$(CXX) $(CXXFLAGS) $(DEBUG) $(SANITIZE) -MMD -MF $(DEP_DIR)/$*.d -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(DEP_DIR):
	mkdir -p $(DEP_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(DEP_DIR)
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME) 6667 1234

-include $(DEPS)

.PHONY: all clean fclean re run
