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
CLANG = clang++

OBJDIR = obj/

SRCDIR = srcs/

INC =	includes

SRC =	Channel.cpp client.cpp Logger.cpp\
		main.cpp Request.cpp server.cpp\
		utils.cpp


# OBJS
OBJ = $(addprefix $(OBJDIR), $(SRC:.cpp=.o))

all: $(NAME)


$(NAME): $(OBJ) $(INC)
	@$(CLANG) $(CPPFLAGS) -o $(NAME) $(OBJ)

$(OBJ): $(OBJDIR)%.o : $(SRCDIR)%.cpp
	@mkdir -p $(OBJDIR)
	@$(CLANG) $(FLAGS) -I$(INC) -o $@ -c $<

clean:
	@rm -rf $(OBJDIR)
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all