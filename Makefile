
NAME = ircserv
SRC	= Server.cpp main.cpp
OBJ	= $(SRC:.cpp=.o)
CFLAGS 	= -Wall -Wextra -Werror -std=c++98
CXX	= c++

all: $(OBJ) $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp 
	$(CXX) $(CFLAGS) -c $< -o $@

fclean: clean
	rm -f $(NAME)

clean:
	rm -f $(OBJ)

re: fclean all

t: re clean
	./$(NAME) 5555 pass

v: re clean
	valgrind ./$(NAME) 5555 pass

.PHONY: all fclean clean re t v
