/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 16:06:47 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/18 20:41:34 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"

int	main(int argc, char **argv){
	if (argc == 3){
        irc_server object;
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

// WARNING 
// getting recv: Resource temporarily unavailable !!!!