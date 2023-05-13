# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/01 20:07:11 by yamzil            #+#    #+#              #
#    Updated: 2023/05/12 23:37:20 by yamzil           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g

SRC =	server.cpp\
		main.cpp\
		client.cpp\
		utils.cpp\
		

INC =	irc.hpp\
		client.hpp\

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ) $(INC)
	@c++ $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	@rm -rf $(OBJ)

fclean:
	@rm -rf $(OBJ) $(NAME)

re: fclean all