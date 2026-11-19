
NAME = ircserv
SRC	= main.cpp
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
a: re clean
	./$(NAME) 5556 pass
b: re clean
	./$(NAME) 5557 pass

.PHONY: all fclean clean re a
