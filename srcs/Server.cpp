/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgmira <sgmira@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:46:26 by yamzil            #+#    #+#             */
/*   Updated: 2023/06/13 17:33:54 by sgmira           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Reply.hpp"
#include "../includes/Request.hpp"

void irc_server::init_sockets(void)
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		std::cerr << "Socket: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void irc_server::ReusableSocket(void)
{
	int flag = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)) == -1)
	{
		std::cerr << "Set Socket: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::non_blocking(void)
{
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::bind_sockets(void)
{
	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(this->portno);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::listenToIncomingconnection()
{
	if (listen(socket_fd, SOMAXCONN) == -1)
	{
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		close(socket_fd);
		exit(EXIT_FAILURE);
	}
}

void irc_server::multipleconnection()
{
	Client Client_data;
	pollfd server_fd;

	server_fd.fd = socket_fd;
	server_fd.events = POLLIN;
	vec_fd.push_back(server_fd);
	while (true)
	{
		poll_fds = poll(&vec_fd[0], vec_fd.size(), -1);
		if (poll_fds == -1)
		{
			std::cerr << "poll: " << std::strerror(errno) << std::endl;
			close(socket_fd);
			exit(EXIT_FAILURE);
		}
		AcceptIncomingconnection(Client_data);
	}
}

void irc_server::AcceptIncomingconnection(Client &Client_data)
{
	char buffer[BUFFER_SIZE];
	for (size_t i = 0; i < vec_fd.size(); i++)
	{
		if (vec_fd[i].revents & POLLIN)
		{
			if (vec_fd[i].fd == this->socket_fd)
			{

				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
				if (accept_fd == -1)
				{
					std::cerr << "accept: " << std::strerror(errno) << std::endl;
					exit(EXIT_FAILURE);
				}
				Client_data.setfd_number(accept_fd);
				Client_data.setIPAddress(inet_ntoa(client_addr.sin_addr));
				guest[accept_fd] = Client_data;
				pollfd client_fd;
				client_fd.fd = accept_fd;
				client_fd.events = POLLIN;
				vec_fd.push_back(client_fd);
			}
			else
			{
				std::memset(&buffer, 0, sizeof(buffer));
				int nbytes = recv(vec_fd[i].fd, buffer, BUFFER_SIZE, 0);
				if (nbytes == -1)
				{
					if (nbytes == EAGAIN ||  nbytes == EWOULDBLOCK)
					{
						continue;
					}
					else
					{
						std::cerr << "recv: " << std::strerror(errno) << std::endl;
					}
				}
				else if (nbytes == 0)
				{
					close(vec_fd[i].fd);
					vec_fd.erase(vec_fd.begin() + i);
					std::cout << "See you later!" << std::endl;
				}
				else
				{
					Request object;
					std::string _message;
					std::string message(buffer);
					guest.at(vec_fd[i].fd).MessageFormat += message;

					size_t pos = guest.at(vec_fd[i].fd).MessageFormat.find_first_of("\r\n");
					while (pos != std::string::npos)
					{
						_message = guest.at(vec_fd[i].fd).MessageFormat.substr(0, pos);
						object.parseRequest(_message);
						if (!object.getcmd().compare(0, object.getcmd().length(), "PASS"))
							PASS(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "NICK"))
							NICK(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "USER"))
							USER(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "KICK"))
							KICK(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "JOIN"))
							JOIN(object.getRequest_(), guest[vec_fd[i].fd]);
						// else if (!object.getcmd().compare(0, object.getcmd().length(), "PRIVMSG"))
						// 	PRIVMSG(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "INVITE"))
							INVITE(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "TOPIC"))
							TOPIC(object.getRequest(), guest[vec_fd[i].fd]);
						// else if (!object.getcmd().compare(0, object.getcmd().length(), "TOPIC"))
						// 	TOPIC(object.getRequest(), guest[vec_fd[i].fd]);
						else if (!object.getcmd().compare(0, object.getcmd().length(), "NAMES"))
						{
							// std::cout << "getcmd "<<  object.getcmd()<<"\n";
							// std::cout << "compare(0, object.getcmd().length(), 'NAMES') "<<  object.getcmd().compare(0, object.getcmd().length(), "NAMES")<<"\n";
							// puts("HRERERERE");
							NAMES(object.getRequest(), guest[vec_fd[i].fd]);;
						}
						else if (!object.getcmd().compare(0, object.getcmd().length(), "MODE"))
							MODE(object.getRequest(), guest[vec_fd[i].fd]);
						_message = guest.at(vec_fd[i].fd).MessageFormat.erase(0, pos + 2);
						pos = guest.at(vec_fd[i].fd).MessageFormat.find_first_of("\r\n");
					}
				}
			}
		}
	}
}

//// COMMANDS

void	irc_server::TOPIC(std::vector<std::string> request, Client& client)
{
	(void) client;
	logger.log(DEBUG, "TOPIC");
	std::vector<Channel>::iterator it = findChannelByName(request[0]);
	std::cout << "request[0] = " << request[0] << "   " <<  "request[1] = " << request[1] << std::endl;
	
	if(it != channels.end())
	{
		logger.log(DEBUG, "Channel Found ... setting topic!");
		it->setChannelTopic(request[1]);
	}
	else
	{
		logger.log(DEBUG, "Channel Not found");
		return ;
	}
}

bool irc_server::checkChannelMask(char c) {
	return (c != '#' && c != '!' && c != '+' && c != '&');
}

e_action irc_server::checkAction(std::string mode) {
	return mode[0] == '+' ? ADD : mode[0] == '-' ? REMOVE : UNKOWN;
}

std::map<int, Client>::iterator irc_server::findClient(std::string nickname) {
	std::map<int, Client>::iterator it = guest.begin();
	for (; it != guest.end(); it++) {
		if (it->second.getNickname() == nickname) {
			return it;
		}
	}
	return guest.end();
}


bool irc_server::isoperator(std::map<std::string, Client&> operators, int userFd)
{
	std::map<std::string, Client&>::iterator it = operators.begin();
	for (; it != operators.end(); it++) {
		if (it->second.getFdNumber() == userFd) {
			return true;
		}
	}
	return false;
}

void	irc_server::INVITE(std::vector<std::string> request, Client& client) 
{	
	logger.log(DEBUG, "INVITE");
	std::vector<Channel>::iterator it = findChannelByName(request[1]);
	// std::cout << "request[1] = " << request[1] << "   " << channels.begin()->getChannelName() << std::endl;
	
	if(it != channels.end())
	{
		logger.log(DEBUG, "Found a channel!");
		if(it->isInviteOnly())
		{
			logger.log(DEBUG, "This channel is Invite Only");
			if(isoperator(it->getOperators(), client.getFdNumber()))
			{
				logger.log(DEBUG, "You're an Operator! You can Proceed ...");
				std::map<int, Client>::iterator it2 = findClient(request[0]);
				it->invite_user(it2->second);
				if (it2 != guest.end()) {
					if (it2->first != client.getFdNumber())
						it->invite_user(it2->second);
					else
						send_message(client.getFdNumber(), "You can't invite yourself\n");
				}
				// else
				// 	send_message(client.getFdNumber(), ERR_NOSUCHNICK(request[1], request[0]));
			}
			else {
				logger.log(DEBUG, "You do not have permission to use INVITE command");
				return ;
			}
			
		}
		else if (it->is_already_join_2(request[1]))
		{
			send_message(client.getFdNumber(), "User is already a member of this channel\n");
			return ;
		}
		else if (!it->is_already_join_2(request[1]) && it->getUserLimit() != -1 && it->getUsersTotal() >= it->getUserLimit()) 
		{
			send_message(client.getFdNumber(), "Error we have reach channel limit\n");
			return ;
		}
	}	
	else
	{
		logger.log(DEBUG, "Cannot find existing channel, creating one ...");
		// std::pair<std::string, std::string> pair = request[i];
		// std::cout << "Channel name: " << pair.first << " key: " << pair.second << std::endl;
		if (!client.getUserName().length()) {
			send_message(client.getFdNumber(), ERR_NOTREGISTERED(client.getNickname()));
			return ;
		}
		if (checkChannelMask(request[1][0])) {
			send_message(client.getFdNumber(), ERR_BADCHANMASK(client.getNickname(), request[1]));
		} else {
			channels.push_back(Channel(request[1], client));
			std::map<int, Client>::iterator cl = findClient(request[0]);
			channels.end()->invite_user(cl->second);
		}
	}
	
}	



// void	irc_server::INVITE(std::vector<std::string> request, Client& client) {
// 	logger.log(DEBUG, "invite command");
// 	std::vector<Channel>::iterator it = findChannelByName(request[0]);
// 	if (it != channels.end()) {
// 		logger.log(DEBUG, "Channel found " + request[0]);
// 		if (it->isAnOperatorOrOwner(client)) {
// 			std::map<int, Client>::iterator u = findClient(request[1]);
// 			if (u != guest.end()) {
// 				if (u->first != client.getFdNumber()) {
// 					it->invite_user(u->second);
// 				} else {
// 					send_message(client.getFdNumber(), "Can not invite your self\n");
// 				}
// 			} else {
// 				send_message(client.getFdNumber(), ERR_NOSUCHNICK(request[1], request[0]));
// 			}
// 		} else {
// 			logger.log(DEBUG, "Action can not be permited");
// 			return ;
// 		}
// 	} else {
// 		logger.log(DEBUG, "Channel not found");
// 		return ;
// 	}
// }


void irc_server::PASS(std::vector<std::string> request, Client &client)
{
	if (!request.size()){
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(std::string("PASS"), client.getNickname()));
		return;
	}
	else if (request[0] != this->passwd){
		send_message(client.getFdNumber(), ERR_PASSWDMISMATCH(client.getNickname(), client.getUserName()));
		return;
	}
	else if (request[0] == this->passwd && !client.getPasswordApproved()){
		std::cout << "Password approved, you can set a nickname" << std::endl;
		client.setPasswordApproved(true);
		return ;
	}
	else if (client.getPasswordApproved() == true){
		send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName()));
		return ;
	}
}


void irc_server::NICK(std::vector<std::string> request, Client &client)
{
	if (!request.size())
	{
		send_message(client.getFdNumber(), ERR_NONICKNAMEGIVEN(client.getNickname(), client.getUserName()));
	}
	else if (check_param(request[0].c_str(), client) && client.getPasswordApproved())
	{
		if (nicknames.find(request[0]) == nicknames.end())
		{
			if (client.getNickNameSited())
			{
				std::set<std::string>::iterator it = nicknames.begin();
				for (; it != nicknames.end(); ++it){
					if (nicknames.find(request[0]) == nicknames.end()){
						nicknames.erase(client.getNickname());
						nicknames.insert(request[0]);
						client.setNickname(request[0]);
						std::cout << "update your nickname to: " << client.getNickname() << std::endl;
						break;
					}
				}
			}
			else
			{
				client.setNickname(request[0]);
				std::cout << "Your Nickname is: " << client.getNickname() << std::endl;
				client.setNicknameSited(true);
				nicknames.insert(request[0]);
			}
		}
		else
		{
			send_message(client.getFdNumber(), ERR_NICKNAMEINUSE(request[0], client.getUserName()));
		}
	}
}

void irc_server::USER(std::vector<std::string> request, Client &client)
{

	if (request.size() != 4)
		// send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "USER", client.getUserName()));
		puts("needs fix");
	else
	{
		if (client.getPasswordApproved() && client.getNickNameSited())
		{
			if (!client.getUserNameSited())
			{
				client.setUserName(request[0]);
				client.setMode(request[1]);
				client.setHostname(request[2]);
				client.setRealName(request[3]);
				client.setUsernameSited(true);
				welcome_message(client.getFdNumber(), RPL_WELCOME(client.getNickname(), client.getUserName(), client.getHostname()));
				welcome_message(client.getFdNumber(), RPL_YOURHOST(client.getNickname(), client.getHostname()));
				get_date();
			}
		}
		else
			send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName()));
	}
}

void	irc_server::NAMES(std::vector<std::string> request, Client& client)
{
	std::cout << "Size: " << request.size() << std::endl;
	// (void) request;
	(void) client;
	logger.log(DEBUG, "names command");
	// std::vector<std::pair<std::string, std::string> >::iterator it;
	// for (size_t i = 0; i < request.size(); i++) {
	// 	std::pair<std::string, std::string> pair = request[i];
	// 	std::cout << "Channel name: " << pair.first << " key: " << pair.second << std::endl;
	// }
	std::cout << "Request size: " << request.size() << std::endl;
	if(!request.size())
	{
		logger.log(DEBUG, "Listing channels");
		std::vector<Channel>::iterator it1;
		for(it1 = channels.begin(); it1 != channels.end(); ++it1)
		{
			std::cout << it1->getChannelName() << std::endl;
			std::map<std::string, Client&>::iterator it2;
			puts("HRERE");
			for (it2 = it1->getUsers().begin(); it2 != it1->getUsers().end(); ++it2)
			{
				// Print the keys
				// std::cout << "Key: " << ch->first << std::endl;
				
				std::cout << "    " << it2->second.getNickname() << std::endl;
			}
		}
	}
	else
	{		
		if (request[0][0] == '#' || request[0][0] == '+' || request[0][0] == '!' || request[0][0] == '&') {
			// CHANNEL MODE
			logger.log(DEBUG, "Searching for a channel");
			std::vector<Channel>::iterator it = findChannelByName(request[0]);
			if (it != channels.end()) {
				// logger.log(DEBUG, "channel found!");
				std::cout << "Channel name:" << it->getChannelName() << std::endl;
				std::map<std::string, Client&>::iterator ch;
				std::cout << "Users: " << std::endl;
				for (ch = it->getUsers().begin(); ch != it->getUsers().end(); ++ch) {
					// Print the keys
					// std::cout << "Key: " << ch->first << std::endl;

					std::cout << "-" << ch->second.getNickname() << std::endl;
				}

			}
		}
	}
	// std::cout << "Size: " << request.size() << std::endl;
}

void irc_server::leaveAllChannels(Client& client) {
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++) {
		if (it->hasUser(client.getNickname())) {
			if (it->isAnOperatorOrOwner(client))
				it->remove_operator(client);
			it->remove_user(client);
		}
	}
}

void irc_server::JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client) {
	//   Numeric Replies:

    //        ERR_NEEDMOREPARAMS: [D]              ERR_BANNEDFROMCHAN: [D]
    //        ERR_INVITEONLYCHAN: [D]              ERR_BADCHANNELKEY: [D]
    //        ERR_CHANNELISFULL: [D]               ERR_BADCHANMASK: [D]
    //        ERR_NOSUCHCHANNEL: [N]               ERR_TOOMANYCHANNELS: [D]
    //        ERR_TOOMANYTARGETS: [N]              ERR_UNAVAILRESOURCE: [N]
    //        RPL_TOPIC: [D]
	if (_request.empty()) {
		send_message(client.getFdNumber(), msg + ERR_NEEDMOREPARAMS(std::string("JOIN"), client.getNickname()));
		return ;
	}
	// if (!client.getUserName().length()) {
	// 	send_message(client.getFdNumber(), msg + ERR_NOTREGISTERED(client.getNickname()));
	// 	return ;
	// }
	if (_request.size() == 1 && _request[0].first == "0") {
		leaveAllChannels(client);
		client.decrementChannelCount();
		return ;
	}
	for (size_t i = 0; i < _request.size(); i++) {
		std::pair<std::string, std::string> pair = _request[i];
		if (checkChannelMask(pair.first[0]) || pair.first.find(",") != std::string::npos || pair.first.find(" ") != std::string::npos) {
			send_message(client.getFdNumber(), msg + ERR_BADCHANMASK(client.getNickname(), pair.first));
		} else {
			std::vector<Channel>::iterator it = findChannelByName(pair.first);
			if (it != channels.end()) {
				if (client.getChannelCount() >= MAX_CHANNELS) {
					send_message(client.getFdNumber(), msg + ERR_TOOMANYCHANNELS(client.getNickname(), pair.first));
					return ;
				}
				logger.log(DEBUG, "User request to join channel: |" + pair.first + "|");
				if (!it->is_already_join(client) &&  it->getUserLimit() != -1 && it->getUsersTotal() >= it->getUserLimit()) {
					send_message(client.getFdNumber(), msg + ERR_CHANNELISFULL(client.getNickname(), pair.first));
					return ;
				}
				if (it->hasKey()) {
					logger.log(DEBUG, "channel has a key");
					if (!it->getChannelKey().compare(0, it->getChannelKey().length(), pair.second)) {
						logger.log(DEBUG, "Channel key is correct");
						if (!it->join_user(msg, host, client)) {
							send_message(client.getFdNumber(), msg + ERR_BANNEDFROMCHAN(client.getNickname(), pair.first));
							return ;
						}
					} else {
						logger.log(DEBUG, "channel key is not valid");
						send_message(client.getFdNumber(), msg + ERR_BADCHANNELKEY(client.getNickname(), pair.first));
						return ;
					}
				} else {
					logger.log(DEBUG, "channel has no key hh");
					if (!it->join_user(msg, host, client)) {
						send_message(client.getFdNumber(), msg + ERR_BANNEDFROMCHAN(client.getNickname(), pair.first));
						return ;
					}
				}
			} else {
				logger.log(DEBUG, "Channel: " + pair.first + " Not found, will create one");
				channels.push_back(Channel(pair.first, client));
			}
		}
	}
}

// void irc_server::JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client)
// {

// 	std::vector<std::pair<std::string, std::string> >::iterator it;
// 	for (it = _request.begin(); it != _request.end(); ++it) {
// 		std::cout << "First element: " << it->first << std::endl;
// 		std::cout << "Second element: " << it->second << std::endl;
// 	}

// 	std::cout << "Client: " << client.getUserName() << std::endl;
// 	if (_request.empty()) {
// 		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN", client.getUserName()));
// 		return;
// 	}
// 	else {
// 		for (it = _request.begin(); it != _request.end(); ++it) {
// 			if (client.getUserName().empty()) {
// 				send_message(client.getFdNumber(), ERR_NOTREGISTERED(client.getNickname()));
// 				return;
// 			}

// 			if ((it->first[0] != '&' && it->first[0] != '#' && it->first[0] != '+' && it->first[0] != '!')
// 				|| (it->first.find(",") != std::string::npos)) {
// 				send_message(client.getFdNumber(), ERR_BADCHANMASK(client.getNickname(), it->first));
// 				return;
// 			}

// 			if (channels.empty()) {
// 				logger.log(DEBUG, client.getNickname());
// 				channels.push_back(Channel(it->first, client));
// 			}
// 			else {
// 				std::vector<Channel>::iterator it2 = findChannelByName(it->first);
// 				if (it2 != channels.end()) {
// 					logger.log(DEBUG, "User request to join this channel");
// 					// if (!it2->join_user(client)) {
// 					// 	send_message(client.getFdNumber(), ERR_BANNEDFROMCHAN(client.getNickname(), it2));
// 					// 	return;
// 					// }
// 				}
// 				else {
// 					channels.push_back(Channel(it->first, client));
// 				}
// 				it2 = findChannelByName(it->first);
// 				logger.log(DEBUG, "channel " + it->first + " has " + std::to_string(it2->getUsers().size()) + " members");
// 			}
// 		}
// 	}
// }


bool searchInSet(std::set<std::string>& myset, std::string& target) 
{
    std::set<std::string>::const_iterator it = std::find(myset.begin(), myset.end(), target);

    if (it != myset.end()) {
        return true;
    } else {
        return false;
    }
}

bool searchInSet2(std::set<std::string>& myset, const std::string& target) {
    std::set<std::string>::const_iterator it = std::find(myset.begin(), myset.end(), target);

    if (it != myset.end()) {
        return true;
    } else {
        return false;
    }
}

bool serachInChannels(std::vector<Channel> channels, const std::string& target)
{
	std::vector<Channel>::const_iterator it;

	for(it = channels.begin(); it != channels.end(); ++it)
	{
		if(it->getChannelName() == target){
        	return true;
    	}	
	}
    return false;
}

// void	irc_server::MODE(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	// logger.log(INFO, "mode command");
	
// 	// std::vector<std::string>::iterator it;
// 	// for (it = request.begin(); it != request.end(); ++it) {
// 	// 	std::cout << "First element: " << it << std::endl;
// 	// 	std::cout << "Second element: " << it << std::endl;
// 	// }
	
// 	// std::cout << request.size() << std::endl;
// 	// if (request.empty())
// 	// {
// 	// 	send_message(client.getFdNumber(), ERR_NONICKNAMEGIVEN(client.getNickname(), client.getUserName()));
// 	// 	return;
// 	// }
	
// 	if (request.size() == 1)
// 	{
// 		std::cout << "___" << request[0] << std::endl;
// 		// if(searchInSet(nicknames, request[0]))
// 		// {
// 		// 	std::cout << "___" << "It's a User Nickname" << std::endl;
// 		// }
// 		if()
// 		else if(serachInChannels(channels, request[0]))
// 			std::cout << "___" << "It's a channel" << std::endl;
// 		else
// 			std::cout << "___" << "Can't find what you're looking for" << std::endl;
// 	}
	
// 	if (request.size() > 1)
// 	{
// 		//parsing
// 		std::cout << request.size() << std::endl;
// 	}
// 	else
// 	{
// 		logger.log(ERROR, "Not enough MODE arguments");
// 	}
	
// 	for(std::vector<int>::size_type i = 0 ; i < request.size(); i++)
// 	{
// 		std::cout << "element " << i << ": "<< request[i] << std::endl;
// 	}
	
	
// }

void print_usermode(std::__1::vector<user_mode_t> modelist, Client client)
{
	std::cout << "This user's current modes: " << std::endl;
	
	// if(client.get_invisible() == true)
	// 	std::cout << "- Invisible" << std::endl;
	// if(client.get_wallops() == true)
	// 	std::cout << "- Receiving wallops" << std::endl;
	for (size_t i = 0; i < modelist.size(); ++i)
	{
		switch (modelist[i])
		{
			case 0:
				if(client.get_invisible() == true)
					std::cout << "- Invisible" << std::endl;
				break;
			case 1:
				if(client.get_wallops() == true)
				std::cout << "- Receiving wallops" << std::endl;
				break;
			case 2:
				std::cout << "- Operator" << std::endl;
				break;
			case 3:
				std::cout << "- Secure connection" << std::endl;
				break;
			case 4:
				std::cout << "- Cloaking" << std::endl;
				break; 
			case 5:
				std::cout << "- Deaf" << std::endl; 
				break;
		}
	}
}

void	irc_server::MODE(std::vector<std::string> request, Client& client) {
	logger.log(DEBUG, "mode command");
		// channel mode
		// . i: Set/remove Invite-only channel
		// · t: Set/remove the restrictions of the TOPIC command to channel
		// operators
		// · k: Set/remove the channel key (password)
		// · o: Give/take channel operator privilege
		// . l: Set/remove the user limit to channel
	if (request[0][0] == '#' || request[0][0] == '+' || request[0][0] == '!' || request[0][0] == '&') {
		// CHANNEL MODE
		logger.log(DEBUG, "Basic implementation of mode operations on channels");
		std::vector<Channel>::iterator it = findChannelByName(request[0]);
		if (it != channels.end()) {
		// 	Numeric Replies:

        //    ERR_NEEDMOREPARAMS              ERR_KEYSET
        //    ERR_NOCHANMODES                 ERR_CHANOPRIVSNEEDED
        //    ERR_USERNOTINCHANNEL            ERR_UNKNOWNMODE
        //    RPL_CHANNELMODEIS
        //    RPL_BANLIST                     RPL_ENDOFBANLIST
        //    RPL_EXCEPTLIST                  RPL_ENDOFEXCEPTLIST
        //    RPL_INVITELIST                  RPL_ENDOFINVITELIST
        //    RPL_UNIQOPIS
			logger.log(DEBUG, "Channel found " + request[0]);
			if (it->isAnOperatorOrOwner(client)) {
				logger.log(DEBUG, "user is an actual owner or operator of the channel");
				std::string mode = request[1];
				if (checkAction(mode) == ADD) {
					logger.log(DEBUG, "mode prefix ADD");
					if (mode == "+i") {
						it->setInviteOnly(true);
					} else if (mode == "+t") {
						//
					} else if (mode == "+k") {
						it->setChannelKey(request[2]);
					} else if (mode == "+o") {
						std::map<int, Client>::iterator u = findClient(request[2]);
						if (u != guest.end()) {
							if (it->hasUser(request[2])) {
									it->add_operator(u->second);
							} else {
								logger.log(DEBUG, "User not found in this channel");
							}
						} else {
							logger.log(DEBUG, "user not found");
						}
					} else if (mode == "+l") {
						it->setChannelLimit(std::atoi(request[2].c_str()));
					}
				}
				else if (checkAction(mode) == REMOVE) {
					logger.log(DEBUG, "mode prefix REMOVE");
					if (mode == "-i") {
						it->setInviteOnly(false);
					} else if (mode == "-t") {
						//
					} else if (mode == "-k") {
						it->setChannelKey("");
					} else if (mode == "-o") {
						std::map<int, Client>::iterator u = findClient(request[2]);
						if (u != guest.end()) {
							if (it->hasUser(request[2])) {
									it->remove_operator(u->second);
							} else {
								logger.log(DEBUG, "User not found in this channel");
							}
						} else {
							logger.log(DEBUG, "user not found");
						}
					} else if (mode == "-l") {
						it->setChannelLimit(-1);
					}
				} else {
					// UNKNOWN mode
					logger.log(DEBUG, "Unkown mode prefix " + mode);
				}
			} else {
				logger.log(DEBUG, "You are not an owner or an operator");
			}
		}
		else {
			logger.log(DEBUG, "Channel not found");
			return ;
		}
	}
	else {
		// USER MODE
		logger.log(DEBUG, "Basic implementation of mode operations on users");
		std::map<int, Client>::iterator cl = findClient(request[0]);
		// std::map<int, Client>::iterator it;
		// for (it = guest.begin(); it != guest.end(); ++it) {
		// 	std::cout << it->second.getNickname() << std::endl;
		// }

		if (cl != guest.end()) {
			// std::cout << "WE FOUND A USER MODE" << std::endl;
			logger.log(DEBUG, "User found " + request[0]);
			std::string mode = request[1];
			if(mode.empty())
			{
				if(cl->second.get_user_mode().empty())
					logger.log(DEBUG, "user" + request[0] + "doesn't have any modes yet");
				else
					print_usermode(cl->second.get_user_mode(), client);
			}
			if(checkAction(mode) == ADD)
			{
				if (mode == "+i")
				{
					logger.log(DEBUG, "Adding invisible mode ...");
					cl->second.set_invisible(true);
					cl->second.get_user_mode().push_back(u_i);
				}
				else if (mode == "+w")
				{
					logger.log(DEBUG, "Adding recieve wallops mode ...");
					cl->second.set_wallops(true);
					cl->second.get_user_mode().push_back(u_w);
				}
			}
			else if(checkAction(mode) == REMOVE)
			{
				if (mode == "-i")
				{
					logger.log(DEBUG, "Removing invisible mode ...");
					cl->second.set_invisible(false);
				}
				else if (mode == "-w")
				{
					logger.log(DEBUG, "Removing recieve wallops mode ...");
					cl->second.set_wallops(false);
				}
			}
		}
	}
}

// void irc_server::JOIN(std::vector<std::pair<std::string, std::string> > _request, Client& client) {
// 	std::cout << "Size: " << _request.size() << std::endl;
// 	for (size_t i = 0; i < _request.size(); i++) {
// 		std::pair<std::string, std::string> pair = _request[i];
// 		std::cout << "Channel name: " << pair.first << " key: " << pair.second << std::endl;
// 		if (!client.getUserName().length()) {
// 			send_message(client.getFdNumber(), ERR_NOTREGISTERED(client.getNickname()));
// 			return ;
// 		}
// 		if (checkChannelMask(pair.first[0]) || pair.first.find(",") != std::string::npos || pair.first.find(" ") != std::string::npos) {
// 			send_message(client.getFdNumber(), ERR_BADCHANMASK(client.getNickname(), pair.first));
// 		} else {
// 			std::vector<Channel>::iterator it = findChannelByName(pair.first);

// 			if (it != channels.end()) {
// 				logger.log(DEBUG, "User request to join channel: |" + pair.first + "|");
// 				if (!it->is_already_join(client) &&  it->getUserLimit() != -1 && it->getUsersTotal() >= it->getUserLimit()) {
// 					send_message(client.getFdNumber(), "Error we have reach channel limit\n");
// 					return ;
// 				}
// 				if (it->hasKey()) {
// 					// provide the key
// 					logger.log(DEBUG, "channel has a key");
// 					if (!it->getChannelKey().compare(0, it->getChannelKey().length(), pair.second)) {
// 						logger.log(DEBUG, "Channel key is correct");
// 						if (!it->join_user(client)) {
// 							send_message(client.getFdNumber(), ERR_BANNEDFROMCHAN(client.getNickname(), pair.first));
// 							return ;
// 						}
// 					} else {
// 						logger.log(DEBUG, "channel key is not valid");
// 						std::string msg = "Need a key to unlock the channel";
// 						send_message(client.getFdNumber(), ERR_BADCHANNELKEY(msg));
// 						return ;
// 					}
// 				} else {
// 					logger.log(DEBUG, "channel has no key hh");
// 					if (!it->join_user(client)) {
// 						send_message(client.getFdNumber(), ERR_BANNEDFROMCHAN(client.getNickname(), pair.first));
// 						return ;
// 					}
// 				}
// 			} else {
// 				logger.log(DEBUG, "Channel: " + pair.first + " Not found, will create one");
// 				channels.push_back(Channel(pair.first, client));
// 			}
// 			it = findChannelByName(pair.first);
// 			logger.log(DEBUG, "channel " + pair.first + " has "+ std::to_string(it->getUsers().size()) + " members");
// 		}
// 	}
// }

// void irc_server::JOIN(std::vector<std::pair<std::string, std::string> > _request, Client &client)
// {
// 	// (void) _request;
// 	// (void)client;
// 	std::cout << "Size: " << _request.size() << std::endl;
// 	if (!_request.size())
// 	{
// 		send_message(client.getFdNumber(), ERR_NONICKNAMEGIVEN(client.getNickname(), client.getUserName()));
// 	}
// 	std::vector<std::pair<std::string, std::string> >::iterator it;
// 	for (it = _request.begin(); it != _request.end(); ++it) {
// 		std::cout << "First element: " << it->first << std::endl;
// 		std::cout << "Second element: " << it->second << std::endl;
// 	}
// 	std::cout << "Client: " << client.getUserName() << std::endl;
// 	if (!_request.size()) {
// 		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN", client.getUserName()));
// 	}
// 	 else 
// 	{
// 		if (!client.getUserName().length()) {
// 			send_message(client.getFdNumber(), ERR_NOTREGISTERED(client.getNickname()));
// 			return ;
// 		}
// 		if (_request[0].first[0] != '&' || _request[0].first[0] != '#' || _request[0].first[0] != '+' || _request[0].first[0] != '!'
// 			|| _request[0].first.find(",") != std::string::npos) {
// 			send_message(client.getFdNumber(), ERR_BADCHANMASK(client.getNickname(), _request[0].first));
// 			return ;
// 		}
// 		// for (size_t i = 0; i < _request.size(); ++i) {
//         // std::cout << "Element " << i << ": " << _request[i].first << ", " << _request[i].second << std::endl;}
// 		if (!channels.size()) {
// 			logger.log(DEBUG, client.getNickname());
// 			channels.push_back(Channel(parametrs, client));
// 		} else {
// 			std::vector<Channel>::iterator it = findChannelByName(parametrs);

// 			if (it != channels.end()) {
// 				logger.log(DEBUG, "User request to join this channel");
// 				if (!it->join_user(client)) {
// 					send_message(client.getFdNumber(), ERR_BANNEDFROMCHAN(client.getNickname(), parametrs));
// 					return ;
// 				}
// 			} else {
// 				channels.push_back(Channel(parametrs, client));
// 			}
// 			it = findChannelByName(parametrs);
// 			logger.log(DEBUG, "channel " + parametrs + " has "+ std::to_string(it->getUsers().size()) + " members");
// 		}
// 	}
// 	}
	
	// logger.log(INFO, parametrs);
	// for (size_t i = 0; i < _request.size(); i++)
	// {
	// 	std::cout << _request[i].first << " : " << _request[i].second << std::endl;
	// 	std::cout << _request[i].first.length() << " : " << _request[i].second.length() << std::endl;
	// }
	// if (!_request.size()) {
	// 	send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN", client.getUserName()));
	// } else {
	// 	// Channel name must be started with '&', '#', '+' or '!' of length up to fifty (50) characters
	// 	// JOIN #test^Dtest
	// 	if (!client.getUserName().length()) {
	// 		send_message(client.getFdNumber(), ERR_NOTREGISTERED(client.getNickname()));
	// 		return ;
	// 	}
	// 	logger.log(INFO, parametrs);
	// 	if (parametrs[0] != '&' || parametrs[0] != '#' || parametrs[0] != '+' || parametrs[0] != '!'
	// 		|| parametrs.find(",") != std::string::npos) {
	// 		send_message(client.getFdNumber(), ERR_BADCHANMASK(client.getNickname(), parametrs));
	// 		return ;
	// 	}
	// 	if (!channels.size()) {
	// 		logger.log(DEBUG, client.getNickname());
	// 		channels.push_back(Channel(parametrs, client));
	// 	} else {
	// 		std::vector<Channel>::iterator it = findChannelByName(parametrs);

	// 		if (it != channels.end()) {
	// 			logger.log(DEBUG, "User request to join this channel");
	// 			if (!it->join_user(client)) {
	// 				send_message(client.getFdNumber(), ERR_BANNEDFROMCHAN(client.getNickname(), parametrs));
	// 				return ;
	// 			}
	// 		} else {
	// 			channels.push_back(Channel(parametrs, client));
	// 		}
	// 		it = findChannelByName(parametrs);
	// 		logger.log(DEBUG, "channel " + parametrs + " has "+ std::to_string(it->getUsers().size()) + " members");
	// 	}
	// }
// }

// void irc_server::PRIVMSG(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	// if (parametrs[0] == '&' || parametrs[0] == '#' || parametrs[0] == '+' || parametrs[0] == '!') {
// 		// PRIVMSG #madrid hello world
// 	// logger.log(DEBUG, "Send messages to channel " + parametrs);
// 	for (size_t i = 0; i < channels.size(); i++) {
// 		std::map<std::string, Client&> users = channels[i].getUsers();
// 		for (std::map<std::string, Client&>::iterator it = users.begin(); it != users.end(); it++) {
// 			send_message(it->second.getFdNumber(), "Hello world\n");
// 		}
// 	}
// }

void irc_server::KICK(std::vector<std::string> request, Client &client)
{
	(void)client;
	for (size_t i = 0; i < request.size(); i++)
	{
		std::cout << request[i] << std::endl;
		std::cout << request[i].size() << std::endl;
	}
	// std::size_t pos = parametrs.find(" ");
	// if (pos != std::string::npos) {
	// 	std::string channelName = parametrs.substr(0, pos);
	// 	parametrs.erase(0, pos + 1);
	// 	// try {
	// 	std::vector<Channel>::iterator it_chan = findChannelByName(channelName);
	// 	if (it_chan != channels.end()) {
	// 		// channel.ban_user(client);
	// 		logger.log(DEBUG, "kick the user " + parametrs);
	// 		std::map<std::string, Client&> users = it_chan->getUsers();
	// 		std::map<std::string, Client&>::iterator it = users.end();
	// 		// logger.log(DEBUG, "User found " + std::to_string(it_chan->getUsers().size()));
	// 		for (std::map<std::string, Client&>::iterator t = users.begin(); t != users.end(); t++) {
	// 			if (t->first == parametrs) {
	// 				it = t;
	// 				break ;
	// 			}
	// 		}
	// 		if (it != users.end()) {
	// 			it_chan->ban_user(it->second);
	// 		} else {
	// 			send_message(client.getFdNumber(), ERR_USERNOTINCHANNEL(client.getNickname(), parametrs, channelName));
	// 			return ;
	// 		}

	// 	} else {
	// 		send_message(client.getFdNumber(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
	// 		return ;
	// 	}
	// 	// } catch(ChannelNotFound e) {
	// 	// }
	// 	// for (size_t i = 0; i < channels.size(); i++) {
	// 	// 	if (!channels[i].getChannelName().compare(0, parametrs.length(), parametrs)) {
	// 	// 		//
	// 	// 	}
	// 	// }
	// }
}

// void	irc_server::INVITE(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	logger.log(INFO, "invite command");
// }

// void	irc_server::TOPIC(std::vector<std::string> request, Client& client) {
// 	(void) client;
// 	logger.log(INFO, "topic command");
// }


/// GETTERS AND SETTERS

int irc_server::getSocketFd(void)
{
	return (this->socket_fd);
}

void irc_server::setSocketFd(int socket_fd)
{
	this->socket_fd = socket_fd;
}

void irc_server::setPassword(std::string passwd)
{
	this->passwd = passwd;
}

std::string irc_server::getPassword()
{
	return (this->passwd);
}

std::vector<Channel>::iterator irc_server::findChannelByName(std::string channel)
{
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->getChannelName() == channel)
		{
			return it;
		}
	}
	return channels.end();
}

irc_server::irc_server() : channels(), logger(Logger::getLogger())
{
}

irc_server::~irc_server()
{
}
