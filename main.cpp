/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 16:06:47 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/09 16:53:11 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

int	main(int argc, char **argv){
	if (argc == 3){
        irc_server  object;
        try{
            object.check_port(argv);
            object.init_sockets();
            object.ReusableSocket();
            object.non_blocking();
            object.bind_sockets();
            object.listenToIncomingconnection();
            object.multipleconnection();
        }
        catch(std::exception &e){
            std::cerr << e.what() << std::endl;
        }
	}
}


// NICKNAME must not contain SPACE, COMMA, ASTRIX, QUESTIN MARK, @ and DOT
//     MUST NOT START WITH $, :, #, &
// CHANNEL NAME MUST NOT CONTAIN SPACE , CTRL-G, COMMA , 