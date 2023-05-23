# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/01 20:07:11 by yamzil            #+#    #+#              #
#    Updated: 2023/05/23 13:30:27 by yamzil           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g

INC =	includes/Server.hpp\
		includes/Client.hpp\
		includes/Channel.hpp\
		includes/Logger.hpp\
		includes/Reply.hpp\

SRC =	srcs/main.cpp\
		srcs/Server.cpp\
		srcs/Client.cpp\
		srcs/Channel.cpp\
		srcs/Logger.cpp\
		srcs/Utils.cpp\
		
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ) $(INC)
	@c++ $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	@rm -rf $(OBJ)

fclean:
	@rm -rf $(OBJ) $(NAME)

re: fclean all