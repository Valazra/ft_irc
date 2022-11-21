NAME = ft_irc

COMPILER = c++

FLAGS = -Wall -Wextra -Werror #--std=c++98

# INC_DIR = incs

SRCS = main.cpp Server.cpp

OBJ = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(COMPILER) $(FLAGS)-c $< -o $@ # -I. 

all:	$(NAME)

$(NAME):	$(OBJ)
		$(COMPILER) $(FLAGS) -o $(NAME) $(OBJ) # -I$(INC_DIR) 

clean:
		rm -rf $(OBJ)

fclean:		clean
		rm -rf $(NAME)

re:		fclean all

.PHONY:	all clean fclean re make
