/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yamzil <yamzil@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 16:46:26 by yamzil            #+#    #+#             */
/*   Updated: 2023/05/20 04:30:17 by yamzil           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"
#include "../includes/client.hpp"

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
				}
				else{
					std::cerr << "Error Argument" << std::endl;
				}
			}
		}	
	}
}

//// COMMANDS

void irc_server::PASS(std::string paramters, Client &client)
{
	if (paramters.length() == 0){
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "PASS"));
		return;
	}
	else if (paramters != this->passwd){
		send_message(client.getFdNumber(), ERR_PASSWDMISMATCH(client.getNickname()));
		return;
	}
	else if (paramters == this->passwd && !client.getPasswordApproved()){
		std::cout << "Password approved, you can set a nickname" << std::endl;
		client.setPasswordApproved(true);
		return ;
	}
	else if (client.getPasswordApproved() == true){
		send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname()));
		return ;
	}
}

void irc_server::NICK(std::string paramters, Client &client)
{
	if (paramters.length() == 0){
		send_message(client.getFdNumber(), ERR_NONICKNAMEGIVEN(client.getNickname()));
	}
	else if (check_param(paramters.c_str(), client) && client.getPasswordApproved()){
		if (nicknames.find(paramters) == nicknames.end()){
			client.setNickname(paramters);
			std::cout << "Your Nickname is: " << client.getNickname() << std::endl;
			client.setNicknameSited(true);
			nicknames.insert(paramters);
		}
		else{
			send_message(client.getFdNumber(), ERR_NICKNAMEINUSE(paramters));
		}
	}
}

void irc_server::USER(std::string parametrs, Client &client){
	if (parametrs.length() == 0){
		send_message(client.getFdNumber(), ERR_NEEDMOREPARAMS(client.getNickname(), "USER"));
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
					welcome_message(client.getFdNumber(), client);
				}
				else
					send_message(client.getFdNumber(), ERR_ALREADYREGISTRED(client.getNickname()));
			}
			else
				send_message(client.getFdNumber(), "Error: An Authentication step has been skipped\n");
		}
		catch(const std::exception& e){
			std::cerr << e.what() << std::endl;
		}
	}
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

irc_server::irc_server(){
}

irc_server::~irc_server(){
}
