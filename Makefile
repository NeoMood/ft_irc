# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayoubaqlzim <ayoubaqlzim@student.42.fr>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/01 20:07:11 by yamzil            #+#    #+#              #
#    Updated: 2023/05/29 15:05:50 by ayoubaqlzim      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

# CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g -fo-stack-protector
CPPFLAGS = -std=c++98 -fsanitize=address
CLANG = c++

OBJDIR = obj/

SRCDIR = srcs/

INC =	includes

SRC =	Channel.cpp Client.cpp Logger.cpp\
		main.cpp Request.cpp Server.cpp\
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