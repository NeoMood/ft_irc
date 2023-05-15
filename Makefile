# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/01 20:07:11 by yamzil            #+#    #+#              #
#    Updated: 2023/05/15 18:41:11 by yamzil           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g

SRC =	server.cpp\
		main.cpp\
		client.cpp\
		utils.cpp\
		commands.cpp\
		

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