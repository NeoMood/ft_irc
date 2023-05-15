/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 01:29:51 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/15 14:58:23 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void irc_server::check_port(char **argv)
{
	std::string port = argv[1];
	std::string password = argv[2];
    if (port < "1024" || port > "65535")
        throw std::out_of_range("Port number out of the bounds");
    this->portno = std::stoi(port);
    this->passwd = password;
}

void    irc_server::send_message(int fd, std::string message){
    write (fd, message.c_str(), message.length());
}