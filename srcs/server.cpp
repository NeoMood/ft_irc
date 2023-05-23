/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:46:26 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/23 13:29:07 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Reply.hpp"

void    irc_server::init_sockets(void){
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1){
        std::cerr << "Socket: " << std::strerror(errno) << std::endl;
        exit (EXIT_FAILURE);
    }
}

void	irc_server::ReusableSocket(void){
	int	flag = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag)) == -1){
		std::cerr << "Set Socket: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::non_blocking(void){
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1){
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::bind_sockets(void){
	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(this->portno);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::listenToIncomingconnection(){
	if (listen(socket_fd, SOMAXCONN) == -1){
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		close (socket_fd);
        exit (EXIT_FAILURE);
	}
}

void	irc_server::multipleconnection(){
	Client	Client_data;
	pollfd	server_fd;

	server_fd.fd = socket_fd;
	server_fd.events = POLLIN;
	vec_fd.push_back(server_fd);
	get_date();
	while (true)
	{
		poll_fds = poll(&vec_fd[0], vec_fd.size(), -1);
		if ( poll_fds == -1){
			std::cerr << "poll: " << std::strerror(errno) << std::endl;
			close (socket_fd);
			exit (EXIT_FAILURE);
		}
		AcceptToIncomingconnection(Client_data);
	}
}

void	irc_server::AcceptToIncomingconnection(Client& Client_data)
{
	char buffer[BUFFER_SIZE];
	for (size_t i = 0; i < vec_fd.size(); i++)
	{
		if (vec_fd[i].revents & POLLIN && i == 0)
		{
			struct sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
			if (accept_fd == -1){
				std::cerr << "accept: " << std::strerror(errno) << std::endl;
				exit (EXIT_FAILURE);
			}
			Client_data.setfd_number(accept_fd);
			Client_data.setIPAddress(inet_ntoa(client_addr.sin_addr));
			guest[accept_fd] = Client_data;
			pollfd client_fd;
			client_fd.fd = accept_fd;
			client_fd.events = POLLIN;
			vec_fd.push_back(client_fd);
		}
		else if (vec_fd[i].revents & POLLIN)
		{
			std::memset(&buffer, 0, sizeof(buffer));
			int nbytes = recv(vec_fd[i].fd, buffer, BUFFER_SIZE, 0);
			if (nbytes == -1) {
				std::cerr << "recv: " << std::strerror(errno) << std::endl;
			}
			else if (nbytes == 0) {
				close(vec_fd[i].fd);
				vec_fd.erase(vec_fd.begin() + i);
				std::cout << "See you later!" << std::endl;
			}
			else 
			{
				std::string message(buffer);
				size_t	pos = message.find(" ");
				if (pos != std::string::npos){
					std::string	command = message.substr(0, pos);
					for (std::string::iterator it = command.begin(); it != command.end(); ++it) {
        				*it = std::toupper(*it);
    				}
					full_command.push_back(command);
					std::string	parameters = message.substr(pos + 1, message.length() - pos - 2);
					full_command.push_back(parameters);
			
					if (!command.compare(0, command.length(), "PASS")){
						PASS(parameters, guest[vec_fd[i].fd]);
					}
					else if (!command.compare(0, command.length(), "NICK")){
						NICK(parameters, guest[vec_fd[i].fd]);
					}
					else if (!command.compare(0, command.length(), "USER")){
						USER(parameters, guest[vec_fd[i].fd]);    
					}
					else if (!command.compare(0, command.length(), "JOIN")) {
						JOIN(parameters, guest[vec_fd[i].fd]);
					}
					else if (!command.compare(0, command.length(), "PRIVMSG")) {
						PRIVMSG(parameters, guest[vec_fd[i].fd]);
					}
					else if (!command.compare(0, command.length(), "KICK")){
						KICK(parameters, guest[vec_fd[i].fd]);
					}
					else if (!command.compare(0, command.length(), "INVITE")){
						INVITE(parameters, guest[vec_fd[i].fd]);    
					}
					else if (!command.compare(0, command.length(), "TOPIC")) {
						TOPIC(parameters, guest[vec_fd[i].fd]);
					}
					else if (!command.compare(0, command.length(), "MODE")) {
						MODE(parameters, guest[vec_fd[i].fd]);
					}
				}
				else{
					send_message(vec_fd[i].fd, "Error: No Enough Arguments\n");
				}
			}
		}	
	}
}

//// COMMANDS

void irc_server::PASS(std::string paramters, Client &client)
{
	if (paramters.length() == 0){
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "PASS", client.getUserName()));
		return;
	}
	else if (paramters != this->passwd){
		send_message(client.getFdNumber(), ERR_PASSWDMISMATCH(client.getNickname(), client.getUserName()));
		return;
	}
	else if (paramters == this->passwd && !client.getPasswordApproved()){
		std::cout << "Password approved, you can set a nickname" << std::endl;
		client.setPasswordApproved(true);
		return ;
	}
	else if (client.getPasswordApproved() == true){
		send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName()));
		return ;
	}
}

void irc_server::NICK(std::string paramters, Client &client)
{
	if (paramters.length() == 0){
		send_message(client.getFdNumber(), ERR_NONICKNAMEGIVEN(client.getNickname(), client.getUserName()));
	}
	else if (check_param(paramters.c_str(), client) && client.getPasswordApproved()){
		if (nicknames.find(paramters) == nicknames.end()){
			client.setNickname(paramters);
			std::cout << "Your Nickname is: " << client.getNickname() << std::endl;
			client.setNicknameSited(true);
			nicknames.insert(paramters);
		}
		else{
			send_message(client.getFdNumber(), ERR_NICKNAMEINUSE(paramters, client.getUserName()));
		}
	}
}

void irc_server::USER(std::string parametrs, Client &client){
	if (parametrs.length() == 0){
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "USER", client.getUserName()));
	}
	else{
		try
		{
			std::string	delim = " ";
			size_t	pos = parametrs.find(delim);
			std::string	user = parametrs.substr(0, pos);
			parametrs.erase(0, pos + delim.length());
			size_t pos_ = parametrs.find(delim);
			std::string	mode = parametrs.substr(0, pos_);
			parametrs.erase(0, pos_ + delim.length());
			size_t pose = parametrs.find(delim);
			std::string hostname = parametrs.substr(0, pose);
			parametrs.erase(0, pose + delim.length());
			std::string real_name = parametrs.substr(1);
			if (client.getPasswordApproved() && client.getNickNameSited())
			{
				if (!client.getUserNameSited()){
					client.setUserName(user);
					client.setMode(mode);
					client.setHostname(hostname);
					client.setRealName(real_name);
					client.setUsernameSited(true);
					welcome_message(client.getFdNumber(), RPL_WELCOME(client.getNickname(), client.getUserName(), client.getHostname()));
					welcome_message(client.getFdNumber(), RPL_YOURHOST(client.getNickname(), client.getHostname()));
				}
				else
					send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname(), client.getUserName()));
			}
			else
				send_message(client.getFdNumber(), "Error: An Authentication step has been skipped\n");
		}
		catch(const std::exception& e){
			std::cerr << e.what() << std::endl;
		}
	}
}

void irc_server::JOIN(std::string parametrs, Client& client) {
	if (!parametrs.length()) {
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN", client.getUserName()));
	} else {
		// Channel name must be started with '&', '#', '+' or '!' of length up to fifty (50) characters
		// JOIN #test^Dtest
		if (!client.getUserName().length()) {
			send_message(client.getFdNumber(), ERR_NOTREGISTERED(client.getNickname()));
			return ;
		}
		logger.log(INFO, parametrs);
		if (parametrs[0] != '&' || parametrs[0] != '#' || parametrs[0] != '+' || parametrs[0] != '!'
			|| parametrs.find(",") != std::string::npos) {
			send_message(client.getFdNumber(), ERR_BADCHANMASK(client.getNickname(), parametrs));
			return ;
		}
		if (!channels.size()) {
			logger.log(DEBUG, client.getNickname());
			channels.push_back(Channel(parametrs, client));
		} else {
			std::vector<Channel>::iterator it = findChannelByName(parametrs);

			if (it != channels.end()) {
				logger.log(DEBUG, "User request to join this channel");
				if (!it->join_user(client)) {
					send_message(client.getFdNumber(), ERR_BANNEDFROMCHAN(client.getNickname(), parametrs));
					return ;
				}
			} else {
				channels.push_back(Channel(parametrs, client));
			}
			it = findChannelByName(parametrs);
			logger.log(DEBUG, "channel " + parametrs + " has "+ std::to_string(it->getUsers().size()) + " members");
		}
	}
}

void irc_server::PRIVMSG(std::string parametrs, Client& client) {
	(void) client;
	(void) parametrs;
	// if (parametrs[0] == '&' || parametrs[0] == '#' || parametrs[0] == '+' || parametrs[0] == '!') {
		// PRIVMSG #madrid hello world
	// logger.log(DEBUG, "Send messages to channel " + parametrs);
	for (size_t i = 0; i < channels.size(); i++) {
		std::map<std::string, Client&> users = channels[i].getUsers();
		for (std::map<std::string, Client&>::iterator it = users.begin(); it != users.end(); it++) {
			send_message(it->second.getFdNumber(), "Hello world\n");
		}
	}
	// }
}

void	irc_server::KICK(std::string parametrs, Client& client) {
	logger.log(INFO, "kick command for channel");
	std::size_t pos = parametrs.find(" ");
	if (pos != std::string::npos) {
		std::string channelName = parametrs.substr(0, pos);
		parametrs.erase(0, pos + 1);
		// try {
		std::vector<Channel>::iterator it_chan = findChannelByName(channelName);
		if (it_chan != channels.end()) {
			// channel.ban_user(client);
			logger.log(DEBUG, "kick the user " + parametrs);
			std::map<std::string, Client&> users = it_chan->getUsers();
			std::map<std::string, Client&>::iterator it = users.end();
			// logger.log(DEBUG, "User found " + std::to_string(it_chan->getUsers().size()));
			for (std::map<std::string, Client&>::iterator t = users.begin(); t != users.end(); t++) {
				if (t->first == parametrs) {
					it = t;
					break ;
				}
			}
			if (it != users.end()) {
				it_chan->ban_user(it->second);
			} else {
				send_message(client.getFdNumber(), ERR_USERNOTINCHANNEL(client.getNickname(), parametrs, channelName));
				return ;
			}
			
		} else {
			send_message(client.getFdNumber(), ERR_NOSUCHCHANNEL(client.getNickname(), channelName));
			return ;
		}
		// } catch(ChannelNotFound e) {
		// }
		// for (size_t i = 0; i < channels.size(); i++) {
		// 	if (!channels[i].getChannelName().compare(0, parametrs.length(), parametrs)) {
		// 		//
		// 	}
		// }
	}
}

void	irc_server::INVITE(std::string parametrs, Client& client) {
	(void) parametrs;
	(void) client;
	logger.log(INFO, "invite command");
}

void	irc_server::TOPIC(std::string parametrs, Client& client) {
	(void) parametrs;
	(void) client;
	logger.log(INFO, "topic command");
}

void	irc_server::MODE(std::string parametrs, Client& client) {
	(void) parametrs;
	(void) client;
	logger.log(INFO, "mode command");
}

/// GETTERS AND SETTERS

int	irc_server::getSocketFd(void){
	return (this->socket_fd);
}

void	irc_server::setSocketFd(int socket_fd){
	this->socket_fd = socket_fd;
}

void	irc_server::setPassword(std::string passwd){
	this->passwd = passwd;
}

std::string	irc_server::getPassword(){
	return(this->passwd);
}

std::vector<Channel>::iterator irc_server::findChannelByName(std::string channel) {
	for (std::vector<Channel>::iterator it = channels.begin() ; it != channels.end(); ++it) {
		if (it->getChannelName() == channel) {
			return it;
		}
	}
	return channels.end();
}

irc_server::irc_server(): channels(), logger(Logger::getLogger()){
}

irc_server::~irc_server(){
}
