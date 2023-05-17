/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 21:54:29 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/17 22:58:28 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"

void irc_server::PASS(std::string paramters, Client &client)
{
	if (paramters.length() == 0){
		send_message(client.getfd_number(), ERR_NEEDMOREPARAMS(client.getName(), "PASS"));
		return;
	}
	else if (paramters != this->passwd){
		send_message(client.getfd_number(), ERR_PASSWDMISMATCH(client.getName()));
		return;
	}
	else if (paramters == this->passwd && client.getisregistred() == false){
		send_message(client.getfd_number(),"Password approved, you can set a nickname\n");
		client.setisregistred(true);
		return ;
	}
	else if (client.getisregistred() == true){
		send_message(client.getfd_number(), ERR_ALREADYREGISTRED(client.getName()));
		return ;
	}
}

void irc_server::NICK(std::string paramters, Client &client)
{
	if (paramters.length() == 0){
		send_message(client.getfd_number(), ERR_NONICKNAMEGIVEN(client.getName()));
	}
	else if (check_param(paramters.c_str(), client) && client.getisregistred()){
		if (nicknames.find(paramters) == nicknames.end()){
			client.setName(paramters);
			nicknames.insert(paramters);
		}
		else{
			send_message(client.getfd_number(), ERR_NICKNAMEINUSE(paramters));
		}
	}
}