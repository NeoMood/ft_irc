/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 01:29:51 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/17 21:37:28 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

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

bool    irc_server::check_param(const char *nickname, Client &client){
    
	nickname = full_command[1].c_str();
	char x = nickname[0];
	for (size_t i = 0; nickname[i]; i++){
		if (nickname[i] == ',' || nickname[i] == '*' || nickname[i] == '?' || nickname[i] == '@' || nickname[i] == '.'){
			send_message(client.getfd_number(), ERR_ERRONEUSNICKNAME(std::string(nickname)));
            return (false);
		}
		else if (std::strlen(nickname) >= 9){
			send_message(client.getfd_number(), ERR_ERRONEUSNICKNAME(std::string(nickname)));
			return (false);
		}
		else if (x == '$' || x == ':' || x == '#' || x == '&'){
			send_message(client.getfd_number(), ERR_ERRONEUSNICKNAME(std::string(nickname)));
			return (false);
		}
	}
	return (true);
}