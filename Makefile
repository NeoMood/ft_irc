# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/01 20:07:11 by yamzil            #+#    #+#              #
#    Updated: 2023/05/18 21:04:05 by yamzil           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

# CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g
CPPFLAGS = -std=c++98

INC =	${wildcard includes/*.hpp}

SRC =	srcs/main.cpp\
		srcs/server.cpp\
		srcs/client.cpp\
		srcs/utils.cpp\
		srcs/Logger.cpp\
		srcs/Channel.cpp
		


OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ) $(INC)
	@c++ $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	@rm -rf $(OBJ)

fclean:
	@rm -rf $(OBJ) $(NAME)

re: fclean all