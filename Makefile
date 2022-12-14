NAME = ircserv

COMPILER = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 #-g

SRCS = main.cpp Server.cpp Client.cpp Command.cpp Channel.cpp

HEADER = main.h Server.hpp Client.hpp Command.hpp Channel.hpp to_string.hpp signals.h

OBJ = $(SRCS:.cpp=.o)

%.o: %.cpp ${HEADER}
	$(COMPILER) $(FLAGS) -c $< -o $@

all:	$(NAME)

$(NAME):	$(OBJ) ${HEADER}
		$(COMPILER) $(FLAGS) -o $(NAME) $(OBJ)

clean:
		rm -rf $(OBJ)

fclean:		clean
		rm -rf $(NAME)

re:		fclean all

.PHONY:	all clean fclean re make
