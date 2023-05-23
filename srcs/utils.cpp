/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 01:29:51 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/23 11:22:57 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

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
    
	for (size_t i = 0; nickname[i]; i++){
		if (nickname[i] == ',' || nickname[i] == '*' || nickname[i] == '?' || nickname[i] == '@' || nickname[i] == '.'){
			send_message(client.getFdNumber(), ERR_ERRONEUSNICKNAME(std::string(nickname), client.getUserName()));
            return (false);
		}
		else if (std::strlen(nickname) >= 9){
			send_message(client.getFdNumber(), ERR_ERRONEUSNICKNAME(std::string(nickname), client.getUserName()));
			return (false);
		}
	}
	return (true);
}

void	irc_server::welcome_message(int fd, std::string message){ 
	write(fd, message.c_str(), message.length());
}

void	irc_server::get_date(void){

	std::time_t t = std::time(0);
  	std::tm* now = std::localtime(&t);
  	std::cout << "This server was created " << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' << now->tm_mday << std::endl;
}