/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 21:54:29 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/16 17:11:53 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "client.hpp"

void irc_server::PASS(std::vector<std::string>& full_command, Client &client)
{
	const std::string	password = full_command[1];
	if (password == this->passwd && client.getisregistred() == false)
	{
		send_message(client.getfd_number(), "Authentication successful. Please provide a nickname.\n");
		client.setisregistred(true);
		return ;
		
	}
	if (client.getisregistred() == true){
		send_message(client.getfd_number(), "Error: Client already registred.\n");
		// std::cout << client.getfd_number() << std::endl;
		return ;
	}
	full_command.clear();
}

void irc_server::NICK(std::vector<std::string>& full_command, Client &client)
{
	const char *nickname = full_command[1].c_str();
	char x = nickname[0];
	for (size_t i = 0; nickname[i]; i++){
		if (nickname[i] == ',' || nickname[i] == '*' || nickname[i] == '?' || nickname[i] == '@' || nickname[i] == '.'){
			send_message(client.getfd_number(), "Error: Nickname has invalid character\n");
			full_command.clear();
			return ;
		}
		else if (std::strlen(nickname) >= 9){
			send_message(client.getfd_number(), "Error: Nickname longer than 9 character\n");
			full_command.clear();
			return ;
		}
		else if (x == '$' || x == ':' || x == '#' || x == '&'){
			send_message(client.getfd_number(), "Error: Nickname has invalid character\n");
			full_command.clear();
			return ;
		}
	}
	if (client.getisregistred()){
		full_command.clear();
		client.setName(full_command[1]);
	}
	std::cout << "Nickname is :" << client.getName() << std::endl;
}