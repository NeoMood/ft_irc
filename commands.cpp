/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 21:54:29 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/15 23:07:08 by yamzil           ###   ########.fr       */
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
		return ;
	}
	full_command.clear();
}

