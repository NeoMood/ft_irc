/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 16:06:47 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/05 19:38:50 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

int	main(int argc, char **argv){
	if (argc == 3){
        irc_server  object;
        try{
            object.check_port(argv);
            object.createServer();
        }
        catch(std::exception &e){
            std::cerr << e.what() << std::endl;
        }
	}
}

