/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 01:29:51 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/12 22:09:14 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"

void irc_server::check_port(char **argv)
{
	std::string port = argv[1];
	std::string password = argv[2];
    if (port < "1024" || port > "65535")
        throw std::out_of_range("Port number out of the bounds");
	for (int i = 0; password[i]; i++){
		if (password[i] < '0' || password[i] > '9')
			throw std::invalid_argument("Password must be a digit");
	}
    this->portno = std::stoi(port);
    this->passwd = password;
}
