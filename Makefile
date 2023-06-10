# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayoubaqlzim <ayoubaqlzim@student.42.fr>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/01 20:07:11 by yamzil            #+#    #+#              #
#    Updated: 2023/06/10 16:51:42 by ayoubaqlzim      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

# CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g -fo-stack-protector
CPPFLAGS = -std=c++98
CLANG = c++

OBJDIR = obj/

SRCDIR = srcs/

INC =	includes

SRC =	Channel.cpp Client.cpp Logger.cpp\
		main.cpp Request.cpp Server.cpp\
		utils.cpp File.cpp FileTransfer.cpp Arg.cpp Bot.cpp


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